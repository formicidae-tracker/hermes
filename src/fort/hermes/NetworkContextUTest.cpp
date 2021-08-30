#include <gtest/gtest.h>

#include "NetworkContext.hpp"
#include "Error.hpp"
namespace fort {
namespace hermes {

class NetworkContextUTest : public ::testing::Test {};

TEST_F(NetworkContextUTest,ConnectionFailure) {
	EXPECT_THROW(NetworkContext("example.com.does-not-exists",12345,false),InternalError);

}

} // namespace hermes
} // namespace fort
