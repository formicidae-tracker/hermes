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

#include "UTestDataUTest.hpp"

class TestSetup : public ::testing::EmptyTestEventListener {
private:
	void OnTestProgramStart(const ::testing::UnitTest &) override {
		fort::hermes::UTestData::Instance();
	}

	void OnTestProgramEnd(const ::testing::UnitTest &) override {
		fort::hermes::UTestData::Release();
	}

	void OnTestStart(const ::testing::TestInfo &) override {}

	void OnTestPartResult(const ::testing::TestPartResult &) override {}

	void OnTestEnd(const ::testing::TestInfo &) override {}
};

int main(int argc,char ** argv) {
	::testing::InitGoogleTest(&argc,argv);

	auto & listeners = ::testing::UnitTest::GetInstance()->listeners();
	listeners.Append(new TestSetup());

	return RUN_ALL_TESTS();
}
