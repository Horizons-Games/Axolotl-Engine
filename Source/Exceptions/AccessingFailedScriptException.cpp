#include "StdAfx.h"

#include "Exceptions/AccessingFailedScriptException.h"

AccessingFailedScriptException::AccessingFailedScriptException(const std::string& message) : message(message)
{
}

const char* AccessingFailedScriptException::what() const noexcept
{
	return message.c_str();
}
