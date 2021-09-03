#include "FileContext.hpp"

#include <stdexcept>
#include <fcntl.h>
#include <sys/stat.h>

#include "Error.hpp"

#include <google/protobuf/util/delimited_message_util.h>

#include <fort/hermes/Header.pb.h>

#include "CheckHeader.hpp"

#ifndef O_BINARY
#define O_BINARY 0
#endif

namespace fort {
namespace hermes {


void FileContext::OpenFile(const std::string & filename) {
	google::protobuf::io::ZeroCopyInputStream * stream = nullptr;
	int fd = open((filename + "unc").c_str(),O_RDONLY | O_BINARY);
	if ( fd > 0 ) {
		d_file = std::make_shared<google::protobuf::io::FileInputStream>(fd);
		d_file->SetCloseOnDelete(true);
		d_gzip.reset();
		stream = d_file.get();
	} else {
		fd = open(filename.c_str(),O_RDONLY | O_BINARY);
		if (fd < 0 ) {
			throw InternalError("On call of open(): " +std::string(strerror(errno)),(fh_error_code_e)errno);
		}
		d_file = std::make_shared<google::protobuf::io::FileInputStream>(fd);
		d_file->SetCloseOnDelete(true);
		d_gzip = std::make_shared<google::protobuf::io::GzipInputStream>(d_file.get());
		stream = d_gzip.get();
	}

	Header h;
	bool cleanEOF = false;
	bool good = google::protobuf::util::ParseDelimitedFromZeroCopyStream(&h,stream,&cleanEOF);
	if ( good == false
	     || cleanEOF == true ) {
		throw InternalError("Stream has no header message",FH_STREAM_NO_HEADER);
	}
	CheckFileHeader(h);

	d_width = h.width();
	d_height = h.height();

	d_path = filename;
}

FileContext::FileContext(const std::string & filename, bool followFiles)
	: d_path(filename)
	, d_width(0)
	, d_height(0)
	, d_followFiles(followFiles) {
	OpenFile(filename);
}

FileContext::~FileContext() {
}

void FileContext::Read(fort::hermes::FrameReadout * ro) {
	google::protobuf::io::ZeroCopyInputStream * stream = d_gzip
		? (google::protobuf::io::ZeroCopyInputStream * ) d_gzip.get()
		: (google::protobuf::io::ZeroCopyInputStream * ) d_file.get();


	if ( stream == nullptr) {
		throw EndOfFile();
	}

	d_line.Clear();
	ro->Clear();
	d_line.set_allocated_readout(ro);
	bool cleanEOF = false;
	bool good = google::protobuf::util::ParseDelimitedFromZeroCopyStream(&d_line,stream,&cleanEOF);
	d_line.release_readout();
	if ( good == false) {
		if (cleanEOF == false ) {
			throw UnexpectedEndOfFileSequence("unexpected EOF while decoding line",
			                                  d_path.string());
		} else {
			throw UnexpectedEndOfFileSequence("missing a footer",d_path.string());
		}
	}

	if ( ro->has_time() ) {
		ro->set_width(d_width);
		ro->set_height(d_height);

		return;
	} else if ( d_line.has_footer() == false ) {
		throw UnexpectedEndOfFileSequence("got an empty line",d_path.string());
	}

	if (d_line.footer().next().length() == 0 || d_followFiles == false) {
		d_gzip.reset();
		d_file.reset();

		throw EndOfFile();
	}

	OpenFile((d_path.parent_path() / d_line.footer().next() ).string());
	Read(ro);
}




} // namespace hermes
} // namespace fort
