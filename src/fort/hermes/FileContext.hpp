#pragma once

#include "Context.hpp"

#include <memory>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/gzip_stream.h>

#include <fort/hermes/Header.pb.h>
#include <filesystem>

namespace fort {
namespace hermes {

class FileContext : public Context {
public:
	FileContext(const std::string & filename, bool followFiles = true);
	virtual ~FileContext();

	void Read(fort::hermes::FrameReadout * ro);


private:
	typedef std::shared_ptr<google::protobuf::io::FileInputStream> FilePtr;
	typedef std::shared_ptr<google::protobuf::io::GzipInputStream> GZipPtr;

	void OpenFile(const std::string & filename);

	std::filesystem::path d_path;

	FilePtr d_file;
	GZipPtr d_gzip;
	FileLine d_line;
	size_t   d_width,d_height;
	bool     d_followFiles;
};

} // namespace hermes
} // namespace fort
