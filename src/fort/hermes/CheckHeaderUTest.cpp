#include <gtest/gtest.h>

#include "CheckHeader.hpp"
#include "Error.hpp"
namespace fort {
namespace hermes {

class CheckHeaderUTest : public ::testing::Test {
};

TEST_F(CheckHeaderUTest,SupportedVersions) {
	std::vector<std::pair<int,int>> versions = {{0,1},{0,2},{0,3},{0,4},{0,5}};
	for ( const auto & [vMajor,vMinor] : versions ) {
		Header header;
		header.set_type(Header_Type_Network);
		header.mutable_version()->set_vmajor(vMajor);
		header.mutable_version()->set_vminor(vMinor);
		EXPECT_NO_THROW(CheckNetworkHeader(header));
	}
	Header header;
	header.set_type(Header_Type_Network);
	header.mutable_version()->set_vmajor(12);
	header.mutable_version()->set_vminor(0);
	try {
		CheckNetworkHeader(header);
		FAIL() << "Expected: CheckNetworkHeader(header)"
		       << "  Throws: InternalError"
		       << "  Actual: it doesn't throw anything";
	} catch ( const InternalError & e ) {
		EXPECT_EQ(e.what(),std::string("unsupported version 12.0"));
		EXPECT_EQ(e.Code(),FH_STREAM_WRONG_VERSION);
	} catch (...) {
		FAIL() << "Expected: CheckNetworkHeader(header)"
		       << "  Throws: InternalError"
		       << "  Actual: it throw another type";
	}
}

TEST_F(CheckHeaderUTest,StreamType) {
	Header header;
	header.mutable_version()->set_vmajor(0);
	header.mutable_version()->set_vminor(1);
	header.set_type(Header_Type_Network);
	EXPECT_NO_THROW(CheckNetworkHeader(header));
	EXPECT_THROW(CheckFileHeader(header),InternalError);
	header.set_type(Header_Type_File);
	header.set_width(1);
	header.set_height(1);
	EXPECT_THROW(CheckNetworkHeader(header),InternalError);
	EXPECT_NO_THROW(CheckFileHeader(header));
}

TEST_F(CheckHeaderUTest,FileHeaderMustContainSize) {
	Header header;
	header.mutable_version()->set_vmajor(0);
	header.mutable_version()->set_vminor(1);
	header.set_type(Header_Type_File);
	header.set_width(0);
	header.set_height(1);
	EXPECT_THROW(CheckFileHeader(header),InternalError);
	header.set_width(1);
	header.set_height(0);
	EXPECT_THROW(CheckFileHeader(header),InternalError);
	header.set_width(1);
	header.set_height(2);
	EXPECT_NO_THROW(CheckFileHeader(header));
}


} // namespace hermes
} // namespace fort
