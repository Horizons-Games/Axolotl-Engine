#include "StdAfx.h"

#include "Exceptions/AccessingFailedScriptException.h"

AccessingFailedScriptException::AccessingFailedScriptException(const std::string& message) : AxoDefaultException(message)
{
}
