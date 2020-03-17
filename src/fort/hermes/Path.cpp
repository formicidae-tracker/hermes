/*
 * Copyright (c) 2012 - 2013 Alexandre Tuleu
 *
 * This file is part of biorob-cpp.
 *
 *   biorob-cpp is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as
 *   published by the Free Software Foundation, either version 3 of the
 *   License, or (at your option) any later version.
 *
 *   biorob-cpp is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of both the GNU General Public
 *   License AND GNU Lesser General Public License along with
 *   biorob-cpp.  If not, see <http://www.gnu.org/licenses/>.
 */


/**
 * Pathname.cpp
 *
 *  Created on: 7 nov. 2012
 *      Author: akay
 */

#include "Path.h"

#include <algorithm>
#include <iostream>
#include <vector>
#include <stdexcept>

using namespace base;

Path::Path()
	: d_separator('/') {
}

Path::Path(const std::string & name)
	: d_path(name)
	, d_separator('/') {
}

Path::~Path() {
}

enum PathElementType {
	ROOT        = 1,
	STANDARD    = 2,
	CURRENT_DIR = 3,
	PARENT_DIR  = 4,

};

struct PathElement {
	PathElementType             Type;
	std::string::const_iterator Start;
	std::string::const_iterator End;

	static PathElement Root() {
		static std::string root = "/";
		return PathElement(root.cbegin(), root.end());
	};

	PathElement(std::string::const_iterator begin, std::string::const_iterator end)
		: Type(STANDARD)
		, Start(begin)
		, End(end) {
		size_t s = (size_t)std::distance(begin,end);
		if (s == 0) {
			throw std::invalid_argument("Element is empty");
		}

		if ( s == 1 ) {
			switch (*begin) {
			case '.' :
				Type = CURRENT_DIR;
				break;
			case '/' :
				Type = ROOT;
				break;
			}
		}

		if ( s == 2  && *begin == '.' && *(begin + 1) == '.' ) {
			Type = PARENT_DIR;
		}
	}

	static void Extract(const std::string & path,
	                    char separator , std::vector<PathElement> & elements) {
		elements.reserve((size_t)std::count(path.begin(),path.end(),separator) + 2);
		auto elementBegin = path.cbegin();


		for ( auto p = path.cbegin(); p != path.cend(); ++p ) {
			if ( *p != separator ) {
				continue;
			}

			if (p == path.cbegin() ) {
				elements.push_back(PathElement::Root());
				continue;
			}

			//we insure we point at beginning of element definition
			if (*elementBegin == separator ) {
				++elementBegin;
			}

			// we have some content
			if ( std::distance(elementBegin,p) == 0) {
				// we have an empty element
				continue;
			}

			elements.push_back(PathElement(elementBegin,p));

			elementBegin = p;
		}

		// do not forget to add the last element !
		if ( *elementBegin == separator ) {
			++elementBegin;
		}
		if (elementBegin != path.cend() ) {
			elements.push_back(PathElement(elementBegin,path.end()));
		}

		// Removes all unucessary parent dir
		for( auto e = elements.begin(); e != elements.end(); ) {
			if ( e == elements.begin() ) {
				++e;
				continue;
			}

			if ( e->Type == CURRENT_DIR ) {
				e = elements.erase(e);
				continue;
			}

			if ( e->Type == PARENT_DIR ) {
				PathElementType parentType = (e-1)->Type;
				if ( parentType == PARENT_DIR ) {
					// we keep the element
					++e;
					continue;
				}
				if ( parentType == ROOT ) {
					//we keep the ROOT element, but we kill ourselve;
					e = elements.erase(e);
					continue;
				}
				// in that case both us and parent should be erased
				e = elements.erase(e - 1, e + 1);
				continue;
			}

			++e;
		}
	}

	static std::string Join(std::vector<PathElement> & elements,
	                        char separator) {
			if (elements.empty() ) {
				return ".";
			}


			size_t expectedSize(0);
			std::for_each(elements.cbegin(),elements.cend(),[&expectedSize](const PathElement & e) {
					expectedSize += (size_t) std::distance(e.Start,e.End);
				});

			std::string path;
			path.reserve(expectedSize + elements.size() - 1);
			auto start = elements.cbegin();
			if ( start->Type == ROOT) {
				path += separator;
				++start;
			}
			for ( auto e = start ; e!= elements.cend(); ++e) {
				if ( e != start ) {
					path += separator;
				}
				path += std::string(e->Start,e->End);
			}

			return path;
	}
};

std::ostream & operator<<(std::ostream & out, const PathElement & e);
std::ostream & operator<<(std::ostream & out, const PathElement & e) {
	out << "{type:";
	switch(e.Type) {
	case ROOT:
		out << "ROOT";
		break;
	case STANDARD:
		out << "STANDARD";
		break;
	case CURRENT_DIR:
		out << "CURRENT";
		break;
	case PARENT_DIR:
		out << "PARENT";
		break;
	default:
		out << "<UNKNOWN>";
	}

	return out << " " << std::string(e.Start,e.End) << "}";
}


Path Path::Clean() const {
	if ( d_path.empty() ) {
		return Path(".");
	}

	std::vector<PathElement> elements;
	PathElement::Extract(d_path,d_separator,elements);


	// we may just have reconstructed an empty path

	return Path(PathElement::Join(elements,d_separator));
}

Path Path::Join(std::initializer_list<std::string> elements) const {
	std::string path = d_path;

	for( auto e = elements.begin();
	     e != elements.end();
	     ++e ) {
		if ( e->empty() ) {
			continue;
		}
		if (path.empty()) {
			path = *e;
			continue;
		}
		if ( ! (e->at(0) == d_separator || path.at(path.size() - 1) == d_separator) ) {
			path += d_separator;
		}
		path += *e;
	}

	if (path.empty()) {
		//keeps empty path, clean we reolve it to "."
		return Path("");
	}

	return Path(path).Clean();
}

const std::string & Path::Str() const {
	return d_path;
}

bool Path::IsAbsolute() const {
	return !d_path.empty() && d_path.at(0) == d_separator;
}

bool Path::IsRelative() const {
	if ( d_path.empty() ) {
		return false;
	}
	return !IsAbsolute();
}

std::string Path::Base() const {
	if (d_path == "/.") {
		return ".";
	}

	std::string path(Clean().Str());
	if ( path == "/" ) {
		return path;
	}

	auto p =  std::find(path.rbegin(),path.rend(),d_separator);

	if ( p == d_path.rend() ) {
		return d_path;
	}

	return  std::string(p.base(),path.end());
}

Path Path::Dir() const {
	if ( d_path.empty() ) {
		return Path(".");
	}

	std::vector<PathElement> elements;
	PathElement::Extract(d_path,d_separator,elements);


	if (d_path.at(d_path.size()-1) == d_separator || elements.empty()) {
		//we just return the cleaned path
		return Path(PathElement::Join(elements,d_separator));
	}

	if ( elements.size() == 1 && elements.at(0).Type == ROOT ) {
		// root of root is root
		return Path("/");
	}

	// we pop back last elements;
	elements.pop_back();

	return Path(PathElement::Join(elements,d_separator));
}

std::string Path::Ext() const {
	std::string base = Base();
	auto p = std::find(base.rbegin(),base.rend(),'.');

	if (p == base.rend() || p.base() == ++base.begin() ) {
		return "";
	}

	return std::string(p.base() - 1,base.end());
}


std::ostream & operator<<(std::ostream & out, const base::Path & p) {
	return out << p.Str();
}
