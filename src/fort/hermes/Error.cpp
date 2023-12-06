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

#include "Error.hpp"

#include <sstream>

namespace fort {
namespace hermes {

InternalError::InternalError(
    const std::string &what, fh_error_code_e code
) noexcept
    : cpptrace::runtime_error{what.c_str()}
    , d_code{code} {}

InternalError::~InternalError() {}

fh_error_code_e InternalError::Code() const {
	return d_code;
}

std::string
BuildWhat(const std::string &reason, const FileLineContext &context) noexcept {
	std::ostringstream oss;
	oss << "unexpected end of file sequence in file "
	    << (std::filesystem::path(context.Directory) / context.Filename);
	if (context.SegmentCount > 0) {
		oss << " ( " << context.SegmentIndex << "/" << context.SegmentCount
		    << ")";
	}
	if (context.LineIndex < context.LineCount) {
		oss << " at line " << context.LineIndex << "/" << context.LineCount;
	} else {
		oss << " at line " << context.LineIndex;
	}
	oss << ": " << reason;
	return oss.str();
}

UnexpectedEndOfFileSequence::UnexpectedEndOfFileSequence(
    const std::string &reason, fort::hermes::FileLineContext &&context
) noexcept
    : cpptrace::runtime_error{BuildWhat(reason, context)}
    , d_context{std::move(context)} {}

const FileLineContext &UnexpectedEndOfFileSequence::FileLineContext() const {
	return d_context;
}

} // namespace hermes
} // namespace fort
