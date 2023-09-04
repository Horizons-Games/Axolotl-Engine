#include "StdAfx.h"

#include "Exceptions/ScriptAssertFailedException.h"

 ScriptAssertFailedException::ScriptAssertFailedException(const std::string& message) : message(message)
{
}

const char* ScriptAssertFailedException::what() const noexcept
{
	return message.c_str();
}
