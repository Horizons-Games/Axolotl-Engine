#pragma once

#include "Exceptions/AxoDefaultException.h"

class AccessingFailedScriptException : public AxoDefaultException
{
public:
	AccessingFailedScriptException(const std::string& message);
	~AccessingFailedScriptException() override = default;
};
