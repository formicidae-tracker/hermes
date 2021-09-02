#include <gtest/gtest.h>

#include "UTestDataUTest.hpp"

#include "FileContext.hpp"
#include "Error.hpp"

namespace fort {
namespace hermes {

class FileContextUTest : public ::testing::Test {};

TEST_F(FileContextUTest,NormalFullReading) {
	const auto & info = UTestData::Instance().NormalSequence();
	FileContext context(info.Segments.front());
	FrameReadout ro;
	for ( const auto & expected : info.Readouts ) {
		EXPECT_NO_THROW(context.Read(&ro));
		EXPECT_READOUT_EQ(ro,expected);
	}
	EXPECT_THROW(context.Read(&ro),EndOfFile);
	EXPECT_THROW(context.Read(&ro),EndOfFile);

}

TEST_F(FileContextUTest,NormalPartialReading) {
	const auto & info = UTestData::Instance().NormalSequence();
	FrameReadout ro;
	size_t j = 0;
	for ( const auto & p : info.Segments ) {
		FileContext context(p,false);
		for ( size_t i = 0 ; i < info.ReadoutsPerSegment; ++i ) {
			EXPECT_NO_THROW(context.Read(&ro));
			EXPECT_READOUT_EQ(ro,info.Readouts[j]);
			j++;
		}
		EXPECT_THROW(context.Read(&ro),EndOfFile);
	}
}


TEST_F(FileContextUTest,TruncatedReadingLosses) {
	const auto & info = UTestData::Instance().TruncatedClassicSequence();
	FileContext context(info.Segments.front());
	FrameReadout ro;
	size_t i;
	for (i = 0; i < info.Readouts.size()-4; ++i ) {
		EXPECT_NO_THROW({
				try {
					context.Read(&ro);
				} catch ( const UnexpectedEndOfFileSequence & e ) {
					ADD_FAILURE() << "Got an unexpected UnexpectedEndOfFileSequence for " << e.SegmentFilePath() << " with i " << i;
				}
			});
		EXPECT_READOUT_EQ(ro,info.Readouts[i]);
	}

	try {
		context.Read(&ro);
		ADD_FAILURE() << "Should throw UnexpectedEndOfFileSequence for i "
		              << i << ", but throw nothing with "
		              << info.Segments.back();
	} catch ( const UnexpectedEndOfFileSequence & e ) {
		EXPECT_EQ(e.SegmentFilePath(),info.Segments.back());
	} catch (...) {
		ADD_FAILURE() << "Should throw UnexpectedEndOfFileSequence, but throw another one";
	}

}


TEST_F(FileContextUTest,TruncatedDualReadingLosses) {
	const auto & info = UTestData::Instance().TruncatedDualStreamSequence();
	FileContext context(info.Segments.front());
	FrameReadout ro;
	size_t i;
	for (i = 0; i < info.Readouts.size()-2; ++i ) {
		EXPECT_NO_THROW({
				try {
					context.Read(&ro);
				} catch ( const UnexpectedEndOfFileSequence & e ) {
					ADD_FAILURE() << "Got an unexpected UnexpectedEndOfFileSequence for " << e.SegmentFilePath() << " with i " << i;
				}
			});
		EXPECT_READOUT_EQ(ro,info.Readouts[i]);
	}

	try {
		context.Read(&ro);
		ADD_FAILURE() << "Should throw UnexpectedEndOfFileSequence for i "
		              << i << ", but throw nothing with "
		              << info.Segments.back();
	} catch ( const UnexpectedEndOfFileSequence & e ) {
		EXPECT_EQ(e.SegmentFilePath(),info.Segments.back());
	} catch (...) {
		ADD_FAILURE() << "Should throw UnexpectedEndOfFileSequence, but throw another one";
	}

}


TEST_F(FileContextUTest,ReportsNoFooterFile) {
	const auto & info = UTestData::Instance().NoFooter();
	FileContext context(info.Segments.back());
	FrameReadout ro;
	for ( size_t i = 0 ; i < info.ReadoutsPerSegment; ++i ) {
		EXPECT_NO_THROW(context.Read(&ro));
	}
	try {
		context.Read(&ro);
		ADD_FAILURE() << "Should throw UnexpectedEndOfFileSequence but throw nothing with "
		              << info.Segments.back();
	} catch ( const UnexpectedEndOfFileSequence & e ) {
		EXPECT_EQ(e.SegmentFilePath(),info.Segments.back());
	} catch (...) {
		ADD_FAILURE() << "Should throw UnexpectedEndOfFileSequence, but throw another one";
	}

}

TEST_F(FileContextUTest,ReportsFileWithoutAnHeader) {
	const auto & info = UTestData::Instance().NoHeader();
	EXPECT_THROW(FileContext(info.Segments.front()),InternalError);
}

TEST_F(FileContextUTest,ReportUnexistingFile) {
	EXPECT_THROW(FileContext(UTestData::Instance().Basepath() / "oops"),InternalError);
}


} // namespace hermes
} // namespace fort
