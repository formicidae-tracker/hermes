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
