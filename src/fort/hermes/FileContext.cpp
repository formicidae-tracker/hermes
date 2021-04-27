#include "FileContext.h"

#include <stdexcept>
#include <fcntl.h>
#include <sys/stat.h>

#include "Error.h"

#include <google/protobuf/util/delimited_message_util.h>

#include <fort/hermes/Header.pb.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif

using namespace fort::hermes;


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
	if ( good == false || cleanEOF == true ) {
		throw InternalError("Stream has no header message",FH_STREAM_NO_HEADER);
	}
	//TODO check version
	if (false) {
		throw InternalError("Stream has an unsupported version",FH_STREAM_WRONG_VERSION);
	}

	// TODO: add some checks ???
	d_width = h.width();
	d_height = h.height();

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

	d_line.Clear();
	bool cleanEOF = false;
	bool good = google::protobuf::util::ParseDelimitedFromZeroCopyStream(&d_line,stream,&cleanEOF);
	if ( good == false) {
		if (cleanEOF == false ) {
			if ( d_gzip == nullptr ) {
				throw UnexpectedEndOfFileSequence();
			} else {
				throw InternalError("Could not decode message",FH_MESSAGE_DECODE_ERROR);
			}
		} else {
			throw EndOfFile();
		}
	}
	if ( d_line.has_readout() ) {
		ro->CopyFrom(d_line.readout());
		ro->set_width(d_width);
		ro->set_height(d_height);

		if ( d_gzip == nullptr ) {
			throw UnexpectedEndOfFileSequence();
		}

		return;
	} else if ( d_line.has_footer() == false ) {
		throw InternalError("Message is empty",FH_MESSAGE_DECODE_ERROR);
	}

	if (d_line.footer().next().length() == 0 || d_followFiles == false) {
		throw EndOfFile();
	}

	OpenFile(d_path.Dir().Join({d_line.footer().next()}).Str());
	Read(ro);
}
