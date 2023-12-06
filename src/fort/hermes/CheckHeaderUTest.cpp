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

#include "gmock/gmock.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "CheckHeader.hpp"
#include "Error.hpp"

namespace fort {
namespace hermes {

class CheckHeaderUTest : public ::testing::Test {};

auto IsTracedDescription(const std::string &start) {
	using ::testing::AllOf;
	using ::testing::HasSubstr;
	using ::testing::StartsWith;

	return AllOf(
	    StartsWith(start),
	    HasSubstr("\nStack trace (most recent call first):\n")
	);
}

TEST_F(CheckHeaderUTest, SupportedVersions) {
	std::vector<std::pair<int, int>> versions =
	    {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}};
	for (const auto &[vMajor, vMinor] : versions) {
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
	} catch (const InternalError &e) {
		EXPECT_THAT(e.what(), IsTracedDescription("unsupported version 12.0"));
		EXPECT_EQ(e.Code(), FH_STREAM_WRONG_VERSION);
	} catch (...) {
		FAIL() << "Expected: CheckNetworkHeader(header)"
		       << "  Throws: InternalError"
		       << "  Actual: it throw another type";
	}
}

TEST_F(CheckHeaderUTest, StreamType) {
	Header header;
	header.mutable_version()->set_vmajor(0);
	header.mutable_version()->set_vminor(1);
	header.set_type(Header_Type_Network);
	EXPECT_NO_THROW(CheckNetworkHeader(header));
	EXPECT_THROW(CheckFileHeader(header), InternalError);
	header.set_type(Header_Type_File);
	header.set_width(1);
	header.set_height(1);
	EXPECT_THROW(CheckNetworkHeader(header), InternalError);
	EXPECT_NO_THROW(CheckFileHeader(header));
}

TEST_F(CheckHeaderUTest, FileHeaderMustContainSize) {
	Header header;
	header.mutable_version()->set_vmajor(0);
	header.mutable_version()->set_vminor(1);
	header.set_type(Header_Type_File);
	header.set_width(0);
	header.set_height(1);
	EXPECT_THROW(CheckFileHeader(header), InternalError);
	header.set_width(1);
	header.set_height(0);
	EXPECT_THROW(CheckFileHeader(header), InternalError);
	header.set_width(1);
	header.set_height(2);
	EXPECT_NO_THROW(CheckFileHeader(header));
}

} // namespace hermes
} // namespace fort
