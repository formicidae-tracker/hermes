#pragma once

#include "Context.hpp"

#include <memory>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/gzip_stream.h>

#include <fort/hermes/Header.pb.h>
#include <filesystem>

namespace fort {
namespace hermes {


/**
 * A context that reads from a hermes file sequence
 */
class FileContext : public Context {
public:
	/**
	 * Open the file sequence starting with filename.
	 *
	 * @param filename the first file of the sequence to open
	 * @param followFiles if false, only filename will be read,
	 *        otherwise the complete sequence until the last segment
	 *        is read by this context.
	 *
	 * @throws InternalError if filename is not a valid hermes file sequence
	 */
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
