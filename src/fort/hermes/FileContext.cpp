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

#include <cstring>
#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <limits>
#include <regex>
#include <stdexcept>
#include <sys/stat.h>

#include <google/protobuf/util/delimited_message_util.h>

#include <fort/hermes/Header.pb.h>

#include "CheckHeader.hpp"
#include "Error.hpp"
#include "FileContext.hpp"
#include "Types.hpp"
#include "fort/hermes/details/FileSequence.hpp"

#ifndef O_BINARY
#define O_BINARY 0
#endif

namespace fort {
namespace hermes {

FileContext::~FileContext() = default;

FileContext::FileContext(FileContext &&other)
    : d_path{other.d_path}
    , d_sequence{std::move(other.d_sequence)}
    , d_file{std::move(other.d_file)}
    , d_gzip{std::move(other.d_gzip)}
    , d_lineIndex{other.d_lineIndex}
    , d_lastFrameID{other.d_lastFrameID}
    , d_width{other.d_width}
    , d_height{other.d_height}
    , d_followFiles{other.d_followFiles} {}

FileContext &FileContext::operator=(FileContext &&other) {
	d_path        = other.d_path;
	d_sequence    = std::move(other.d_sequence);
	d_file        = std::move(other.d_file);
	d_gzip        = std::move(other.d_gzip);
	d_lineIndex   = other.d_lineIndex;
	d_lastFrameID = other.d_lastFrameID;
	d_width       = other.d_width;
	d_height      = other.d_height;
	d_followFiles = other.d_followFiles;
	return *this;
}

std::string FileContext::UncompressedFilename(const std::filesystem::path &path
) {
	return path.parent_path() / ("uncompressed-" + path.filename().string());
}

void FileContext::OpenFile(const std::string &filename) {
	google::protobuf::io::ZeroCopyInputStream *stream = nullptr;

#ifndef NDEBUG
	std::cerr << "libhermes [INFO]: checking file '" +
	                 UncompressedFilename(filename) + "'"
	          << std::endl;
#endif

	int fd =
	    open((UncompressedFilename(filename)).c_str(), O_RDONLY | O_BINARY);
	if (fd > 0) {
		d_file = std::make_unique<google::protobuf::io::FileInputStream>(fd);
		d_file->SetCloseOnDelete(true);
		d_gzip.reset();
		stream = d_file.get();
#ifndef NDEBUG
		std::cerr << "libhermes [INFO]: using uncompressed file '" +
		                 UncompressedFilename(filename) + "'"
		          << std::endl;
#endif
	} else {
		fd = open(filename.c_str(), O_RDONLY | O_BINARY);
		if (fd < 0) {
			throw InternalError(
			    "On call of open(): " + std::string(strerror(errno)),
			    (fh_error_code_e)errno
			);
		}
		d_file = std::make_unique<google::protobuf::io::FileInputStream>(fd);
		d_file->SetCloseOnDelete(true);
		d_gzip =
		    std::make_unique<google::protobuf::io::GzipInputStream>(d_file.get()
		    );
		stream = d_gzip.get();
	}

	Header h;
	bool   cleanEOF = false;
	bool   good     = google::protobuf::util::ParseDelimitedFromZeroCopyStream(
        &h,
        stream,
        &cleanEOF
    );
	if (good == false || cleanEOF == true) {
		throw InternalError(
		    "Stream has no header message",
		    FH_STREAM_NO_HEADER
		);
	}
	CheckFileHeader(h);

	d_width  = h.width();
	d_height = h.height();

	d_path      = filename;
	d_lineIndex = 0;
}

FileContext::FileContext(const std::string &filename, bool followFiles)
    : d_path(filename)
    , d_sequence{std::make_unique<details::FileSequence>(filename)}
    , d_width(0)
    , d_height(0)
    , d_lastFrameID{std::numeric_limits<size_t>::max()}
    , d_followFiles(followFiles) {
	OpenFile(filename);
}

void FileContext::Read(fort::hermes::FrameReadout *ro) {
	google::protobuf::io::ZeroCopyInputStream *stream =
	    d_gzip ? (google::protobuf::io::ZeroCopyInputStream *)d_gzip.get()
	           : (google::protobuf::io::ZeroCopyInputStream *)d_file.get();

	if (stream == nullptr) {
		throw EndOfFile();
	}

	d_line.Clear();
	ro->Clear();
	d_line.set_allocated_readout(ro);
	bool cleanEOF = false;
	bool good     = google::protobuf::util::ParseDelimitedFromZeroCopyStream(
        &d_line,
        stream,
        &cleanEOF
    );
	d_line.release_readout();
	if (good == false) {
		if (cleanEOF == false) {
			throw UnexpectedEndOfFileSequence(
			    "could not decode line",
			    d_sequence->FileLineContext(d_path, d_lineIndex, d_lastFrameID)
			);
		} else {
			throw UnexpectedEndOfFileSequence(
			    "missing a footer",
			    d_sequence->FileLineContext(d_path, d_lineIndex, d_lastFrameID)
			);
		}
	}
	++d_lineIndex;

	if (ro->has_time()) {
		ro->set_width(d_width);
		ro->set_height(d_height);
		d_sequence->UpdateSegment(d_path, d_lineIndex - 1, ro->frameid());
		d_lastFrameID = ro->frameid();
	} else if (d_line.has_footer() == false) {
		throw UnexpectedEndOfFileSequence(
		    "got an empty line",
		    d_sequence->FileLineContext(d_path, d_lineIndex, d_lastFrameID)
		);
	}

	if (d_line.has_footer() == false) {
		return;
	}

	d_sequence->Persist();

	if (d_line.footer().next().length() == 0 || d_followFiles == false) {
		d_gzip.reset();
		d_file.reset();

		throw EndOfFile();
	}

	OpenFile((d_path.parent_path() / d_line.footer().next()).string());
	Read(ro);
}

} // namespace hermes
} // namespace fort
