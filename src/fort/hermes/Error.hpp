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

#include "Types.hpp"
#include "hermes.h"

#include <filesystem>

#include <cpptrace/cpptrace.hpp>

namespace fort {
namespace hermes {

/**
 * Represents any kinds of errors with hermes file sequence or streams
 */
class InternalError : public cpptrace::runtime_error {
public:
	InternalError(const std::string &what, fh_error_code_e code) noexcept;
	virtual ~InternalError();

	/**
	 * @returns the associated fh_error_code_e
	 */
	fh_error_code_e Code() const;

private:
	fh_error_code_e d_code;
};

/**
 * Exception thrown by Context which does not contains data anymore.
 */
class EndOfFile : public std::exception {
public:
	EndOfFile() noexcept {}

	virtual ~EndOfFile() {}

	virtual const char *what() const noexcept {
		return "EOF";
	};
};

/**
 * Exception thrown by FileContext when an error occured while reading
 * a file sequence. Such errors happens when during acquisition of the
 * data, the disk became full and the file where truncated.
 */
class UnexpectedEndOfFileSequence : public cpptrace::runtime_error {
public:
	UnexpectedEndOfFileSequence(
	    const std::string &reason, FileLineContext &&context
	) noexcept;

	/**
	 * @returns the segment path where the error occured
	 */
	const fort::hermes::FileLineContext &FileLineContext() const;

private:
	struct FileLineContext d_context;
};

/**
 * Exception throw by non-blockinf NetworkContext when no message are
 * ready in the context.
 */
class WouldBlock : public std::exception {
public:
	WouldBlock() noexcept {}

	virtual ~WouldBlock() {}

	virtual const char *what() const noexcept {
		return "Would block";
	};
};

} // namespace hermes
} // namespace fort
