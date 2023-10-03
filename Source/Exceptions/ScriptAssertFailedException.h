#pragma once

#include "Exceptions/AxoDefaultException.h"

class ScriptAssertFailedException : public AxoDefaultException
{
public:
	ScriptAssertFailedException(const std::string& message);
	~ScriptAssertFailedException() override = default;
};
