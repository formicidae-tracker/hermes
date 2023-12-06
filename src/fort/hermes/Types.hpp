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

#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace fort {
namespace hermes {
struct FileLineContext {
	std::string Filename;
	std::string Directory;

	std::optional<std::filesystem::path> Next;

	size_t PreviousFrameID = 0;
	size_t SegmentIndex    = 0;
	size_t SegmentCount    = 0;

	size_t LineIndex = 0;
	size_t LineCount = 0;
};

} // namespace hermes
} // namespace fort
