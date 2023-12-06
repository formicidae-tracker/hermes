// libfort-hermes - Tracking File I/O library.
//
// Copyright (C) 2018-2023  Universitée de Lausanne
//
// This file is part of libfort-hermes.
//
// libfort-hermes is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// libfort-hermes is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// libfort-hermes.  If not, see <http://www.gnu.org/licenses/>.

#include "CheckHeader.hpp"

#include "Error.hpp"

namespace fort {
namespace hermes {

void CheckVersion(const Version &version) {
	std::set<std::pair<int, int>> supportedVersions = {
	    {0, 1},
	    {0, 2},
	    {0, 3},
	    {0, 4},
	    {0, 5},
	};

	if (supportedVersions.count(
	        std::make_pair(version.vmajor(), version.vminor())
	    ) == 0) {
		throw InternalError(
		    "unsupported version " + std::to_string(version.vmajor()) + "." +
		        std::to_string(version.vminor()),
		    FH_STREAM_WRONG_VERSION
		);
	}
}

void CheckFileHeader(const Header &header) {
	CheckVersion(header.version());
	if (header.type() != Header_Type_File) {
		throw InternalError("wrong header type", FH_STREAM_NO_HEADER);
	}
	if (header.width() == 0) {
		throw InternalError("missing width", FH_STREAM_NO_HEADER);
	}
	if (header.height() == 0) {
		throw InternalError("missing height", FH_STREAM_NO_HEADER);
	}
}

void CheckNetworkHeader(const Header &header) {
	CheckVersion(header.version());
	if (header.type() != Header_Type_Network) {
		throw InternalError("wrong header type", FH_STREAM_NO_HEADER);
	}
}

} // namespace hermes
} // namespace fort
