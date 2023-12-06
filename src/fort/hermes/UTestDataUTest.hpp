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

#pragma once

#include <filesystem>
#include <gtest/gtest.h>
#include <memory>
#include <vector>

#include <fort/hermes/FrameReadout.pb.h>

namespace fort {
namespace hermes {

class UTestData {
public:
	static const UTestData &Instance();
	static void             Release();

	static std::filesystem::path TempDirPath();

	struct SequenceInfo {
		std::vector<std::filesystem::path> Segments;
		std::vector<FrameReadout>          Readouts;
		size_t                             ReadoutsPerSegment;
	};

	UTestData(const std::filesystem::path &basepath);
	~UTestData();

	const std::filesystem::path &Basepath() const;

	const SequenceInfo &NormalSequence() const;
	const SequenceInfo &TruncatedClassicSequence() const;
	const SequenceInfo &TruncatedDualStreamSequence() const;
	const SequenceInfo &NoFooter() const;
	const SequenceInfo &NoHeader() const;

private:
	void WriteSequenceInfos(const std::filesystem::path &path);

	static std::unique_ptr<UTestData> s_instance;

	std::filesystem::path d_basepath;

	SequenceInfo d_normal, d_noFooter, d_truncated, d_truncatedDual, d_noHeader;
};

} // namespace hermes
} // namespace fort

::testing::AssertionResult AssertReadoutEqual(const char * aExpr,
                                              const char * bExpr,
                                              const fort::hermes::FrameReadout & a,
                                              const fort::hermes::FrameReadout & b);

#define EXPECT_READOUT_EQ(a,b) EXPECT_PRED_FORMAT2(AssertReadoutEqual,a,b);
