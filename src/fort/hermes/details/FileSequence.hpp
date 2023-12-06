#pragma once

#include <filesystem>
#include <map>

#include <fort/hermes/Types.hpp>
#include <optional>

namespace fort {
namespace hermes {
namespace details {

struct SegmentInfo {
	std::string Name;
	size_t      Index;
	size_t      LineCount;
	size_t      Start;
	size_t      End;

	std::optional<std::filesystem::path> Next;
};

class FileSequence {
public:
	const static std::string SegmentIndexFilename;

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

	const std::map<std::string, SegmentInfo> &Segments() const noexcept;

private:
	std::filesystem::path IndexFilePath() const;
	void                  BuildFromIndexFile();

	void RebuildIndex();
	void ListFromDirectory();

	std::map<std::string, SegmentInfo> d_segments;
	std::filesystem::path              d_directory;
	bool                               d_needUpdate = false;
};
} // namespace details

} // namespace hermes
} // namespace fort
