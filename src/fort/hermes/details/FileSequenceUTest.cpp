#include "gtest/gtest.h"
#include <gtest/gtest.h>

#include <filesystem>

#include "FileSequence.hpp"
#include "fort/hermes/Error.hpp"
#include "fort/hermes/FileContext.hpp"
#include "fort/hermes/FrameReadout.pb.h"
#include <fort/hermes/UTestDataUTest.hpp>
#include <string>

namespace fort {
namespace hermes {
namespace details {

class FileSequenceUTest : public ::testing::Test {};

TEST_F(FileSequenceUTest, CreateEmptyIndex) {
	auto segmentPath =
	    UTestData::Instance().NormalSequence().Segments.front().parent_path() /
	    FileSequence::SegmentIndexFilename;
	std::filesystem::remove(segmentPath);

	const auto &normalSequence = UTestData::Instance().NormalSequence();

	FileSequence infos{normalSequence.Segments.front()};

	ASSERT_EQ(infos.Segments().size(), normalSequence.Segments.size());
	size_t i = 0;
	for (const auto &s : normalSequence.Segments) {
		SCOPED_TRACE(s.string());
		ASSERT_EQ(infos.Segments().count(s), 1);
		EXPECT_EQ(infos.Segments().at(s).Name, s.filename());
		EXPECT_EQ(infos.Segments().at(s).Index, i++);
		EXPECT_EQ(infos.Segments().at(s).LineCount, 0);
		EXPECT_EQ(infos.Segments().at(s).Start, 0);
		EXPECT_EQ(infos.Segments().at(s).End, 0);
	}
}

void readAll(const std::filesystem::path &path) {
	FileContext seq{path};

	while (true) {
		fort::hermes::FrameReadout ro;
		try {
			seq.Read(&ro);
		} catch (fort::hermes::EndOfFile &) {
			return;
		} catch (const std::exception &e) {
			ADD_FAILURE() << "unexpected exception: " << e.what();
		}
	}
}

TEST_F(FileSequenceUTest, RebuildIndexWhenFullyRead) {
	const auto &normalSequence = UTestData::Instance().NormalSequence();

	readAll(normalSequence.Segments.front());

	FileSequence infos{normalSequence.Segments.front()};

	ASSERT_EQ(infos.Segments().size(), normalSequence.Segments.size());
	size_t index{0};

	for (const auto &s : normalSequence.Segments) {
		SCOPED_TRACE(s.string());
		ASSERT_EQ(infos.Segments().count(s.string()), 1);
		EXPECT_NO_THROW({
			EXPECT_EQ(infos.At(s).Name, s.filename());
			EXPECT_EQ(infos.At(s).Index, index);
			EXPECT_EQ(infos.At(s).LineCount, 10);
			EXPECT_EQ(infos.At(s).Start, 10 * index++ + 1234);
			EXPECT_EQ(infos.At(s).End, 10 * index + 1234);
		});
	};
}

} // namespace details
} // namespace hermes
} // namespace fort
