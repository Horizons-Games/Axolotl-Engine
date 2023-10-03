#include "StdAfx.h"

#include "Exceptions/AxoDefaultException.h"

AxoDefaultException::AxoDefaultException(const std::string& message) : message(message)
{
}

const char* AxoDefaultException::what() const noexcept
{
	return message.c_str();
}
