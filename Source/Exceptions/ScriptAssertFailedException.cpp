#include "StdAfx.h"

#include "Exceptions/ScriptAssertFailedException.h"

ScriptAssertFailedException::ScriptAssertFailedException(const std::string& message) : AxoDefaultException(message)
{
}
