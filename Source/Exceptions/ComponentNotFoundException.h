#pragma once

#include "Exceptions/AxoDefaultException.h"

class ComponentNotFoundException : public AxoDefaultException
{
public:
	ComponentNotFoundException(const std::string& message);
	~ComponentNotFoundException() override = default;
};
