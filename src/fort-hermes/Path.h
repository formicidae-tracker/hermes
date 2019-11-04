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
 * Pathname.h
 *
 *  Created on: 7 nov. 2012
 *      Author: akay
 */

#pragma once

#include <string>
#include <stdint.h>
#include <iosfwd>

namespace base {

// Path handling abstraction
//
// Class to compute and handle Path, as in URL / URI. Could be on some
// platform be similar to a <Filepath>
//
//
class Path {
public:
	// Empty constructor
	Path();
	// Defined constructore
	// @name the path definition
	Path(const std::string & name);
	// standard destructor
	virtual ~Path();

	// Cleans the <Path>
	//
	// Performs the following operation
	// 1. Replaces multiple slashes with a single slash
	// 2. Eliminates each inner .
	// 3. Eliminates each inner ..
	// 4. Eliminates .. at the beginning of the <Path>
	//
	// This operation is intensive
	Path Clean() const;

	// Joins elements to the <os::Path>
	// @element the list of elements to join
	//
	// @return a new Path with the element added.
	Path Join(std::initializer_list<std::string> elements) const;

	// the path, unmodified
	//
	// @return a reference to the  string passed at construction
	const std::string & Str() const;

	// Checks if <Path> is absolute
	//
	// @return true if <CompletePath> is absolute
	virtual bool IsAbsolute() const;
	// Checks if path is relative
	//
	// @return true if <CompletePath> is relative
	bool IsRelative() const;

	// Gives the last part of the <os::Path>
	//
	// Gives the basename part of the <os::Path>. i.e. in most cases
	// the part after the last '/'. Any final '/' is ignored by this
	// function
	//
	// @return a string describing the filename part of the <os::Path>
	std::string Base() const;

	// Gives the directory of the <os::Path>
	//
	// @return a <os::Path> describing the parent of the current
	//         <os::Path>
	Path Dir() const;

	// Gives the suffix of the <os::Path>
	//
	// Gives the extension part of the <os::Path>, last part of the
	// <os::Path> starting with the final dot '.' (and including that
	// dot). if a file starts with a dot (hidden file on UNIX), it
	// ignores that '.', i.e <base::Path::Ext> of '.bashrc' is ''.
	//
	// @return a string describing the extension of the <os::Path>
	std::string Ext() const;
protected :

	std::string d_path;
	char        d_separator;
};


class Filepath : public Path {
public:
	Filepath();
	Filepath(const std::string & name);

	virtual bool IsAbsolute() const;
	


};

}//namesapce base


std::ostream & operator<<( std::ostream & out, const base::Path & p);


