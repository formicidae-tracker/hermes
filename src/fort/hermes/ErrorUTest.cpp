#include "gmock/gmock.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "Error.hpp"

namespace fort {
namespace hermes {

class ErrorsUTest : public ::testing::Test {};

using ::testing::AllOf;
using ::testing::HasSubstr;
using ::testing::StartsWith;

TEST_F(ErrorsUTest, EndOfFileFormatting) {
	EXPECT_EQ(EndOfFile().what(), "EOF");
}

TEST_F(ErrorsUTest, InternalErrorFormatting) {
	InternalError e("foo", fh_error_code_e(42));
	EXPECT_THAT(
	    e.what(),
	    AllOf(
	        StartsWith("foo"),
	        HasSubstr("\nStack trace (most recent call first):")
	    )
	);
	EXPECT_EQ(e.Code(), 42);
}

TEST_F(ErrorsUTest, UnexpectedEndOfSequenceFormatting) {
	UnexpectedEndOfFileSequence e("something is wrong", "/in/the/sound");
	EXPECT_THAT(
	    e.what(),
	    AllOf(
	        StartsWith("Unexpected end of file sequence in '/in/the/sound': "
	                   "something is wrong"),
	        HasSubstr("\nStack trace (most recent call first):")
	    )
	);
	EXPECT_EQ(e.SegmentFilePath(), "/in/the/sound");
}

TEST_F(ErrorsUTest, WouldBlockFormatting) {
	EXPECT_EQ(WouldBlock().what(), std::string("Would block"));
}

} // namespace hermes
} // namespace fort
