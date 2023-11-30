#pragma once

#include <filesystem>
#include <map>

#include <fort/hermes/Types.hpp>

namespace fort {
namespace hermes {
namespace details {

struct SegmentInfo {
	std::string Name;
	size_t      Index;
	size_t      LineCount;
	size_t      Start;
	size_t      End;
};

class FileSequence {
public:
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

private:
	std::filesystem::path IndexFilePath() const;
	void                  BuildFromIndexFile();

	void ListFromDirectory();

	std::map<std::string, SegmentInfo> d_segments;
	std::filesystem::path              d_directory;
	bool                               d_needUpdate = false;
};
} // namespace details

} // namespace hermes
} // namespace fort
