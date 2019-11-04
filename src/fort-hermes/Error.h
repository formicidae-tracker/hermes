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

class WouldBlock : public std::exception {
public:
	WouldBlock() noexcept{}
	virtual ~WouldBlock(){}
	virtual const char * what() const noexcept { return "Would block"; };
};


} // namespace hermes
} // namespace fort
