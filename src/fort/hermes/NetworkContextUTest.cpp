// libfort-hermes - Tracking File I/O library.
//
// Copyright (C) 2018-2023  Universitée de Lausanne
//
// This file is part of libfort-hermes.
//
// libfort-hermes is free software: you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option) any
// later version.
//
// libfort-hermes is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License along with
// libfort-hermes.  If not, see <http://www.gnu.org/licenses/>.

#include <gtest/gtest.h>

#include "Error.hpp"
#include "NetworkContext.hpp"

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

TEST_F(NetworkContextUTest, NormalOperation) {
	const auto  &readouts = UTestData::Instance().NormalSequence().Readouts;
	StreamServer server(
	    {.Port        = 12345,
	     .Begin       = readouts.begin(),
	     .End         = readouts.end(),
	     .WriteHeader = true}
	);
	usleep(1000);
	std::shared_ptr<NetworkContext> context;
	try {
		context = std::make_shared<NetworkContext>("localhost", 12345, false);
	} catch (const std::exception &e) {
		FAIL() << "got unexpected exception: " << e.what();
	}
	FrameReadout ro;
	for (const auto &expected : readouts) {
		EXPECT_NO_THROW(context->Read(&ro));
		EXPECT_READOUT_EQ(ro, expected);
	}
	EXPECT_THROW(context->Read(&ro), EndOfFile);
}

TEST_F(NetworkContextUTest, NoHeader) {
	const auto  &readouts = UTestData::Instance().NormalSequence().Readouts;
	StreamServer server(
	    {.Port        = 12345,
	     .Begin       = readouts.begin(),
	     .End         = readouts.end(),
	     .WriteHeader = false}
	);
	usleep(1000);
	std::shared_ptr<NetworkContext> context;
	EXPECT_THROW(
	    {
		    context =
		        std::make_shared<NetworkContext>("localhost", 12345, false);
	    },
	    InternalError
	);
}

} // namespace hermes
} // namespace fort
