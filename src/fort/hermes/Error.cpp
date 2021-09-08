#include "Error.hpp"


namespace fort{
namespace hermes{

InternalError::InternalError(const std::string & what, fh_error_code_e code) noexcept
	: std::runtime_error(what)
	, d_code(code) {
}

InternalError::~InternalError() {
}

fh_error_code_e InternalError::Code() const {
	return d_code;
}

} // namespace hermes
} // namespace fort