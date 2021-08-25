#include <gtest/gtest.h>

#include "Error.hpp"

namespace fort {
namespace hermes {

class ErrorsUTest : public ::testing::Test {};

TEST_F(ErrorsUTest,EndOfFileFormatting) {
	EXPECT_EQ(EndOfFile().what(),std::string("EOF"));
}

TEST_F(ErrorsUTest,InternalErrorFormatting) {
	InternalError e("foo",fh_error_code_e(42));
	EXPECT_EQ(e.what(),std::string("foo"));
	EXPECT_EQ(e.Code(),42);
}

TEST_F(ErrorsUTest,UnexpectedEndOfSequenceFormatting) {
	UnexpectedEndOfFileSequence e("something is wrong", "/in/the/sound");
	EXPECT_EQ(e.what(),
	          std::string("Unexpected end of file sequence in '/in/the/sound': something is wrong"));
	EXPECT_EQ(e.SegmentFilePath(),"/in/the/sound");
}

TEST_F(ErrorsUTest,WouldBlockFormatting) {
	EXPECT_EQ(WouldBlock().what(),std::string("Would block"));
}


} // namespace hermes
} // namespace fort
