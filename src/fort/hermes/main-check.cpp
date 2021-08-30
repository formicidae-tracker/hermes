#include <gtest/gtest.h>

#include "UTestDataUTest.hpp"

class TestSetup : public ::testing::EmptyTestEventListener {
private:
	void OnTestProgramStart(const ::testing::UnitTest & ) override {
		fort::hermes::UTestData::Instance();
	}
	void OnTestProgramEnd(const ::testing::UnitTest & ) override {
		fort::hermes::UTestData::Release();
	}

	void OnTestStart(const ::testing::TestInfo &) override {}
	void OnTestPartResult(const ::testing::TestPartResult &) override {}
	void OnTestEnd(const ::testing::TestInfo& ) override {}
};


int main(int argc,char ** argv) {
	::testing::InitGoogleTest(&argc,argv);

	auto & listeners = ::testing::UnitTest::GetInstance()->listeners();
	listeners.Append(new TestSetup());

	return RUN_ALL_TESTS();
}
