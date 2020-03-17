#pragma once

#include "Context.h"

#include <memory>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/gzip_stream.h>

#include <fort/hermes/Header.pb.h>
#include "Path.h"

namespace fort {

namespace hermes {

class FileContext : public Context {
public:
	FileContext(const std::string & filename);
	virtual ~FileContext();

	void Read(fort::hermes::FrameReadout * ro);


private:
	typedef std::shared_ptr<google::protobuf::io::FileInputStream> FilePtr;
	typedef std::shared_ptr<google::protobuf::io::GzipInputStream> GZipPtr;

	void OpenFile(const std::string & filename);

	base::Path d_path;

	FilePtr d_file;
	GZipPtr d_gzip;
	FileLine d_line;
	size_t   d_width,d_height;
};

} // namespace hermes

} // namespace fort
