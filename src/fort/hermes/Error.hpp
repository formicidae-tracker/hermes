#pragma once

#include "hermes.h"

#include <stdexcept>

namespace fort {
namespace hermes {

// A wrapper to represent internal error. Any c++ method are only
// authorized to throw these kind of exception.
class InternalError : public std::runtime_error {
public:
	InternalError(const std::string & what, fh_error_code_e code) noexcept;
	virtual ~InternalError();

	fh_error_code_e Code() const;
private:
	fh_error_code_e d_code;
};

class EndOfFile : public std::exception {
public:
	EndOfFile() noexcept{}
	virtual ~EndOfFile(){}
	virtual const char * what() const noexcept { return "EOF"; };
};

class UnexpectedEndOfFileSequence : public std::exception {
public:
	UnexpectedEndOfFileSequence(const std::string & what, const std::string & segmentFilePath) noexcept
		: d_what("Unexpected end of file sequence in '" + segmentFilePath + "': " + what) {
	}
	virtual ~UnexpectedEndOfFileSequence() {}

	virtual const char * what() const noexcept {
		return d_what.c_str();
	}

	const std::string & SegmentFilePath() const {
		return d_segmentFilePath;
	}


private:
	std::string d_segmentFilePath;
	std::string d_what;
};

class WouldBlock : public std::exception {
public:
	WouldBlock() noexcept{}
	virtual ~WouldBlock(){}
	virtual const char * what() const noexcept { return "Would block"; };
};


} // namespace hermes
} // namespace fort
