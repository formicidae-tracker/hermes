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
	UnexpectedEndOfFileSequence e(
	    "something is wrong",
	    {
	        .Filename  = "sound",
	        .Directory = "the",
	        .LineIndex = 2,
	        .LineCount = 2,
	    }
	);
	EXPECT_THAT(
	    e.what(),
	    AllOf(
	        StartsWith("unexpected end of file sequence in file \"the/sound\" "
	                   "at line 2: something is wrong"),
	        HasSubstr("\nStack trace (most recent call first):")
	    )
	);
	EXPECT_EQ(e.FileLineContext().Filename, "sound");
	EXPECT_EQ(e.FileLineContext().Directory, "the");
}

TEST_F(ErrorsUTest, WouldBlockFormatting) {
	EXPECT_EQ(WouldBlock().what(), std::string("Would block"));
}

} // namespace hermes
} // namespace fort
