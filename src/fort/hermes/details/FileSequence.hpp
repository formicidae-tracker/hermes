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
#include <map>

#include <fort/hermes/Types.hpp>
#include <optional>

namespace fort {
namespace hermes {
namespace details {

struct SegmentInfo {
	std::string Name;
	size_t      Index;
	size_t      LineCount;
	size_t      Start;
	size_t      End;

	std::optional<std::filesystem::path> Next;
};

class FileSequence {
public:
	const static std::string SegmentIndexFilename;

	FileSequence(const std::filesystem::path &path);

	const SegmentInfo &At(const std::filesystem::path &path) const;

	void UpdateSegment(
	    const std::filesystem::path &path,
	    size_t                       currentLineIndex,
	    size_t                       currentFrameNumber
	);

	const std::filesystem::path &Directory() const;

	void Persist();

	fort::hermes::FileLineContext FileLineContext(
	    const std::filesystem::path &path,
	    size_t                       lineIndex,
	    size_t                       previousFrame
	);

	const std::map<std::string, SegmentInfo> &Segments() const noexcept;

private:
	std::filesystem::path IndexFilePath() const;
	void                  BuildFromIndexFile();

	void RebuildIndex();
	void ListFromDirectory();

	std::map<std::string, SegmentInfo> d_segments;
	std::filesystem::path              d_directory;
	bool                               d_needUpdate = false;
};
} // namespace details

} // namespace hermes
} // namespace fort
