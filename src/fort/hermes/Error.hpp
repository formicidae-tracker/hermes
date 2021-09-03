#pragma once

#include "hermes.h"

#include <stdexcept>

namespace fort {
namespace hermes {

/**
 * Represents any kinds of errors with hermes file sequence or streams
 */
class InternalError : public std::runtime_error {
public:
	InternalError(const std::string & what, fh_error_code_e code) noexcept;
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
	EndOfFile() noexcept{}
	virtual ~EndOfFile(){}
	virtual const char * what() const noexcept { return "EOF"; };
};

/**
 * Exception thrown by FileContext when an error occured while reading
 * a file sequence. Such errors happens when during acquisition of the
 * data, the disk became full and the file where truncated.
 */
class UnexpectedEndOfFileSequence : public std::exception {
public:
	UnexpectedEndOfFileSequence(const std::string & what, const std::string & segmentFilePath) noexcept
		: d_what("Unexpected end of file sequence in '" + segmentFilePath + "': " + what)
		, d_segmentFilePath(segmentFilePath) {
	}
	virtual ~UnexpectedEndOfFileSequence() {}

	virtual const char * what() const noexcept {
		return d_what.c_str();
	}
	/**
	 * @returns the segment path where the error occured
	 */
	const std::string & SegmentFilePath() const {
		return d_segmentFilePath;
	}


private:
	std::string d_segmentFilePath;
	std::string d_what;
};

/**
 * Exception throw by non-blockinf NetworkContext when no message are
 * ready in the context.
 */
class WouldBlock : public std::exception {
public:
	WouldBlock() noexcept{}
	virtual ~WouldBlock(){}
	virtual const char * what() const noexcept { return "Would block"; };
};


} // namespace hermes
} // namespace fort
