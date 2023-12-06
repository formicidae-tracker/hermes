#include <gtest/gtest.h>

#include "UTestDataUTest.hpp"

#include "Error.hpp"
#include "FileContext.hpp"
#include "hermes.h"

namespace fort {
namespace hermes {

class FileContextUTest : public ::testing::Test {};

TEST_F(FileContextUTest, NormalFullReading) {
	const auto  &info = UTestData::Instance().NormalSequence();
	FileContext  context(info.Segments.front());
	FrameReadout ro;
	for (const auto &expected : info.Readouts) {
		EXPECT_NO_THROW(context.Read(&ro));
		EXPECT_READOUT_EQ(ro, expected);
	}
	EXPECT_THROW(context.Read(&ro), EndOfFile);
	EXPECT_THROW(context.Read(&ro), EndOfFile);
}

TEST_F(FileContextUTest, NormalPartialReading) {
	const auto  &info = UTestData::Instance().NormalSequence();
	FrameReadout ro;
	size_t       j = 0;
	for (const auto &p : info.Segments) {
		FileContext context(p, false);
		for (size_t i = 0; i < info.ReadoutsPerSegment; ++i) {
			EXPECT_NO_THROW(context.Read(&ro));
			EXPECT_READOUT_EQ(ro, info.Readouts[j]);
			j++;
		}
		EXPECT_THROW(context.Read(&ro), EndOfFile);
	}
}

TEST_F(FileContextUTest, TruncatedReadingLosses) {
	const auto  &info     = UTestData::Instance().TruncatedClassicSequence();
	const auto  &infoDual = UTestData::Instance().TruncatedDualStreamSequence();
	FileContext  context(info.Segments.front());
	FrameReadout ro;

	auto checkExceptionThrown = [](const auto &info) {
		FileContext  context(info.Segments.front());
		FrameReadout ro;
		size_t       size;
		try {
			for (size = 0; size < info.Readouts.size(); ++size) {
				context.Read(&ro);
			}
		} catch (UnexpectedEndOfFileSequence e) {
			return std::make_tuple(size, e);
		} catch (const std::exception &e) {
			throw std::runtime_error{
			    std::string("it throw another: ") + e.what()};
		}
		throw std::runtime_error{"it throw nothing"};
	};

	size_t classic, dual;
	try {
		const auto &[classic_, e] = checkExceptionThrown(info);

		classic = classic_;

		EXPECT_EQ(e.FileLineContext().Next, info.Segments[2]);
	} catch (const std::exception &e) {
		ADD_FAILURE() << e.what();
	}

	try {
		const auto &[dual_, e] = checkExceptionThrown(infoDual);

		dual = dual_;

		EXPECT_FALSE(e.FileLineContext().Next.has_value());
	} catch (const std::exception &e) {
		ADD_FAILURE() << e.what();
	}

	EXPECT_GT(dual, classic);
}

TEST_F(FileContextUTest, ReportsNoFooterFile) {
	const auto  &info = UTestData::Instance().NoFooter();
	FileContext  context{info.Segments.back()};
	FrameReadout ro;
	for (size_t i = 0; i < info.ReadoutsPerSegment; ++i) {
		EXPECT_NO_THROW(context.Read(&ro));
	}
	try {
		context.Read(&ro);
		ADD_FAILURE() << "Should throw UnexpectedEndOfFileSequence but "
		                 "throw nothing with "
		              << info.Segments.back();
	} catch (const UnexpectedEndOfFileSequence &e) {
		EXPECT_EQ(
		    e.FileLineContext().Filename,
		    info.Segments.back().filename()
		);
		EXPECT_EQ(
		    e.FileLineContext().Directory,
		    info.Segments.back().parent_path().filename()
		);
		EXPECT_FALSE(e.FileLineContext().Next.has_value());
	} catch (...) {
		ADD_FAILURE() << "Should throw UnexpectedEndOfFileSequence, but "
		                 "throw another one";
	}
}

TEST_F(FileContextUTest, ReportsFileWithoutAnHeader) {
	const auto &info = UTestData::Instance().NoHeader();
	EXPECT_THROW(FileContext(info.Segments.front()), InternalError);
}

TEST_F(FileContextUTest, ReportUnexistingFile) {
	EXPECT_THROW(
	    FileContext(UTestData::Instance().Basepath() / "oops"),
	    InternalError
	);
}

} // namespace hermes
} // namespace fort
