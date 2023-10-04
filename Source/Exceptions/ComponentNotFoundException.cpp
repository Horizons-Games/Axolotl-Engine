#include "StdAfx.h"

#include "Exceptions/ComponentNotFoundException.h"

ComponentNotFoundException::ComponentNotFoundException(const std::string& message) : AxoDefaultException(message)
{
}
