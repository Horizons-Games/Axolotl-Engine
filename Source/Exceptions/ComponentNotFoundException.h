#pragma once

#include <exception>

class ComponentNotFoundException : public std::exception
{
public:
	ComponentNotFoundException(const std::string& message);
	~ComponentNotFoundException() override = default;

	const char* what() const noexcept override;

private:
	std::string message;
};
