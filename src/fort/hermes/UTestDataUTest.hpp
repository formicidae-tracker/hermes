#pragma once

#include <filesystem>
#include <memory>
#include <vector>

#include <fort/hermes/FrameReadout.pb.h>

namespace fort {
namespace hermes {

class UTestData {
public:
	static const UTestData & Instance();
	static void Release();

	static std::filesystem::path TempDirPath();


	struct SequenceInfo {
		std::vector<std::filesystem::path> Segments;
		std::vector<FrameReadout>          Readouts;
	};

	UTestData(const std::filesystem::path & basepath);
	~UTestData();

	const SequenceInfo & NormalSequence() const;
	const SequenceInfo & TruncatedClassisSequence() const;
	const SequenceInfo & TruncatedDualStreamSequence() const;
	const SequenceInfo & NoFooter() const;

private:

	void WriteSequenceInfos(const std::filesystem::path & path);

	static std::unique_ptr<UTestData> s_instance;

	std::filesystem::path d_basepath;

	SequenceInfo d_normal,d_noFooter,d_truncated,d_truncatedDual;
};

} // namespace hermes
} // namespace fort
