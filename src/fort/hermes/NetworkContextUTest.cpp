#include <gtest/gtest.h>

#include "NetworkContext.hpp"
#include "Error.hpp"

#include "StreamServerUTest.hpp"
#include "UTestDataUTest.hpp"

namespace fort {
namespace hermes {

class NetworkContextUTest : public ::testing::Test {};

TEST_F(NetworkContextUTest, ConnectionFailure) {
	try {
		NetworkContext("example.com.does-not-exists", 12345, false);
	} catch (const InternalError &e) {

		return;
	} catch (const std::exception &e) {
		FAIL() << "Unexpected exception: " << e.what();
	}
	FAIL() << "should throw an exception";
}

TEST_F(NetworkContextUTest,NormalOperation) {
	const auto & readouts = UTestData::Instance().NormalSequence().Readouts;
	StreamServer server({.Port = 12345,
	                     .Begin = readouts.begin(),
	                     .End = readouts.end(),
	                     .WriteHeader = true});
	usleep(1000);
	std::shared_ptr<NetworkContext> context;
	try {
		context = std::make_shared<NetworkContext>("localhost",12345,false);
	} catch (const std::exception & e ) {
		FAIL() << "got unexpected exception: " << e.what();
	}
	FrameReadout ro;
	for ( const auto & expected : readouts ) {
		EXPECT_NO_THROW(context->Read(&ro));
		EXPECT_READOUT_EQ(ro,expected);
	}
	EXPECT_THROW(context->Read(&ro),EndOfFile);
}

TEST_F(NetworkContextUTest,NoHeader) {
	const auto & readouts = UTestData::Instance().NormalSequence().Readouts;
	StreamServer server({.Port = 12345,
	                     .Begin = readouts.begin(),
	                     .End = readouts.end(),
	                     .WriteHeader = false});
	usleep(1000);
	std::shared_ptr<NetworkContext> context;
	EXPECT_THROW({
			context = std::make_shared<NetworkContext>("localhost",12345,false);
		},InternalError);
}


} // namespace hermes
} // namespace fort
