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

#include "gtest/gtest.h"
#include <gtest/gtest.h>

#include <filesystem>

#include "FileSequence.hpp"
#include "fort/hermes/Error.hpp"
#include "fort/hermes/FileContext.hpp"
#include "fort/hermes/FrameReadout.pb.h"
#include <fort/hermes/UTestDataUTest.hpp>
#include <string>

namespace fort {
namespace hermes {
namespace details {

class FileSequenceUTest : public ::testing::Test {};

TEST_F(FileSequenceUTest, CreateEmptyIndex) {
	auto segmentPath =
	    UTestData::Instance().NormalSequence().Segments.front().parent_path() /
	    FileSequence::SegmentIndexFilename;
	std::filesystem::remove(segmentPath);

	const auto &normalSequence = UTestData::Instance().NormalSequence();

	FileSequence infos{normalSequence.Segments.front()};

	ASSERT_EQ(infos.Segments().size(), normalSequence.Segments.size());
	size_t i = 0;
	for (const auto &s : normalSequence.Segments) {
		SCOPED_TRACE(s.string());
		ASSERT_EQ(infos.Segments().count(s), 1);
		EXPECT_EQ(infos.Segments().at(s).Name, s.filename());
		EXPECT_EQ(infos.Segments().at(s).Index, i++);
		EXPECT_EQ(infos.Segments().at(s).LineCount, 0);
		EXPECT_EQ(infos.Segments().at(s).Start, 0);
		EXPECT_EQ(infos.Segments().at(s).End, 0);
	}
}

void readAll(const std::filesystem::path &path) {
	FileContext seq{path};

	while (true) {
		fort::hermes::FrameReadout ro;
		try {
			seq.Read(&ro);
		} catch (fort::hermes::EndOfFile &) {
			return;
		} catch (const std::exception &e) {
			ADD_FAILURE() << "unexpected exception: " << e.what();
		}
	}
}

TEST_F(FileSequenceUTest, RebuildIndexWhenFullyRead) {
	const auto &normalSequence = UTestData::Instance().NormalSequence();

	readAll(normalSequence.Segments.front());

	FileSequence infos{normalSequence.Segments.front()};

	ASSERT_EQ(infos.Segments().size(), normalSequence.Segments.size());
	size_t index{0};

	for (const auto &s : normalSequence.Segments) {
		SCOPED_TRACE(s.string());
		ASSERT_EQ(infos.Segments().count(s.string()), 1);
		EXPECT_NO_THROW({
			EXPECT_EQ(infos.At(s).Name, s.filename());
			EXPECT_EQ(infos.At(s).Index, index);
			EXPECT_EQ(infos.At(s).LineCount, 10);
			EXPECT_EQ(infos.At(s).Start, 10 * index++ + 1234);
			EXPECT_EQ(infos.At(s).End, 10 * index + 1234);
		});
	};
}

TEST_F(FileSequenceUTest, IndicatesNextFile) {
	const auto &normalSequence = UTestData::Instance().NormalSequence();

	FileSequence infos{normalSequence.Segments.front()};

	ASSERT_EQ(infos.Segments().size(), normalSequence.Segments.size());
	size_t i = 0;
	for (const auto &s : normalSequence.Segments) {
		SCOPED_TRACE(s.string());
		ASSERT_EQ(infos.Segments().count(s), 1);
		EXPECT_EQ(infos.Segments().at(s).Name, s.filename());
		EXPECT_EQ(infos.Segments().at(s).Index, i++);
		if (i < normalSequence.Segments.size()) {
			EXPECT_NO_THROW({
				EXPECT_EQ(
				    infos.Segments().at(s).Next.value(),
				    normalSequence.Segments[i]
				);
			});
		} else {
			EXPECT_FALSE(infos.Segments().at(s).Next.has_value());
		}
	}
}

} // namespace details
} // namespace hermes
} // namespace fort
