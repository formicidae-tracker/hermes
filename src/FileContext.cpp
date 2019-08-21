#include "FileContext.h"

#include <stdexcept>
#include <fcntl.h>
#include <sys/stat.h>

#include "Error.h"

#include <google/protobuf/util/delimited_message_util.h>

#include "Header.pb.h"

#ifndef O_BINARY
#define O_BINARY 0
#endif

using namespace fort::hermes;


void FileContext::OpenFile(const std::string & filename) {
	int fd = open(filename.c_str(),O_RDONLY | O_BINARY);
	if (fd < 0 ) {
		throw InternalError("On call of open(): " +std::string(strerror(errno)),(fh_error_code_e)errno);
	}
	d_file = std::make_shared<google::protobuf::io::FileInputStream>(fd);
	d_file->SetCloseOnDelete(true);
	d_gzip = std::make_shared<google::protobuf::io::GzipInputStream>(d_file.get());

	Header h;
	bool cleanEOF = false;
	bool good = google::protobuf::util::ParseDelimitedFromZeroCopyStream(&h,d_gzip.get(),&cleanEOF);
	if ( good == false || cleanEOF == true ) {
		throw InternalError("Stream has no header message",FH_STREAM_NO_HEADER);
	}
	//TODO check version
	if (false) {
		throw InternalError("Stream has an unsupported version",FH_STREAM_WRONG_VERSION);
	}

}



FileContext::FileContext(const std::string & filename)
	: d_path(filename) {
	OpenFile(filename);
}

FileContext::~FileContext() {
}

void FileContext::Read(fort::hermes::FrameReadout * ro) {
	d_line.Clear();
	bool cleanEOF = false;
	bool good = google::protobuf::util::ParseDelimitedFromZeroCopyStream(&d_line,d_gzip.get(),&cleanEOF);
	if ( good == false) {
		if (cleanEOF == false ) {
			throw InternalError("Could not decode message",FH_MESSAGE_DECODE_ERROR);
		} else {
			throw EndOfFile();
		}
	}
	if ( d_line.has_readout() ) {
		ro->CopyFrom(d_line.readout());
		return;
	} else if ( d_line.has_footer() == false ) {
		throw InternalError("Message is empty",FH_MESSAGE_DECODE_ERROR);
	}

	if (d_line.footer().next().length() == 0 ) {
		throw EndOfFile();
	}

	OpenFile(d_path.Dir().Join({d_line.footer().next()}).Str());
}
