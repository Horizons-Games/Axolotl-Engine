#pragma once

#include <exception>

class AccessingFailedScriptException : public std::exception
{
public:
	AccessingFailedScriptException(const std::string& message);
	~AccessingFailedScriptException() override = default;

	const char* what() const noexcept override;

private:
	std::string message;
};
