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

#include "Context.hpp"

#include <memory>

#include <google/protobuf/io/gzip_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <filesystem>
#include <fort/hermes/Header.pb.h>

namespace fort {
namespace hermes {

namespace details {
class FileSequence;
}

/**
 * A context that reads from a hermes file sequence
 */
class FileContext : public Context {
public:
	static std::string UncompressedFilename(const std::filesystem::path &path);
	/**
	 * Open the file sequence starting with filename.
	 *
	 * @param filename the first file of the sequence to open
	 * @param followFiles if false, only this filename will be read,
	 *        otherwise the complete sequence until the last segment
	 *        is read by this context.
	 *
	 * @throws InternalError if filename is not a valid hermes file sequence
	 */
	FileContext(const std::string &filename, bool followFiles = true);

	virtual ~FileContext();

	FileContext(FileContext &&other);
	FileContext &operator=(FileContext &&other);

	void Read(fort::hermes::FrameReadout *ro);

private:
	using FilePtr = std::unique_ptr<google::protobuf::io::FileInputStream>;
	using GZipPtr = std::unique_ptr<google::protobuf::io::GzipInputStream>;
	using FileSequencePtr = std::unique_ptr<details::FileSequence>;
	void OpenFile(const std::string &filename);

	std::filesystem::path d_path;

	FileSequencePtr d_sequence;

	FilePtr  d_file;
	GZipPtr  d_gzip;
	FileLine d_line;
	size_t   d_lineIndex   = 0;
	size_t   d_lastFrameID = 0;
	size_t   d_width       = 0;
	size_t   d_height      = 0;
	bool     d_followFiles = false;
};

} // namespace hermes
} // namespace fort
