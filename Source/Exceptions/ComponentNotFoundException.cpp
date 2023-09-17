#include "StdAfx.h"

#include "Exceptions/ComponentNotFoundException.h"

 ComponentNotFoundException::ComponentNotFoundException(const std::string& message) : message(message)
{
}

const char* ComponentNotFoundException::what() const noexcept
{
	return message.c_str();
}
