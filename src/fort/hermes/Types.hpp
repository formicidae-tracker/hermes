#pragma once

#include <filesystem>
#include <optional>
#include <string>

namespace fort {
namespace hermes {
struct FileLineContext {
	std::string Filename;
	std::string Directory;

	std::optional<std::filesystem::path> Next;

	size_t PreviousFrameID = 0;
	size_t SegmentIndex    = 0;
	size_t SegmentCount    = 0;

	size_t LineIndex = 0;
	size_t LineCount = 0;
};

} // namespace hermes
} // namespace fort
