#include "exceptions.h"

GeneralError::GeneralError()
{}

const char* GeneralError::what() const noexcept
{
	return "server responded with an error";
}

UserError::UserError(const std::string& err_msg)
: _err_msg(err_msg) {
}

const char* UserError::what() const noexcept
{
	return _err_msg.c_str();
}
