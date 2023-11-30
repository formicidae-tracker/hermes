#pragma once

#include "Context.hpp"

#include <memory>

#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/gzip_stream.h>

#include <fort/hermes/Header.pb.h>
#include <filesystem>

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
