#pragma once

#include <exception>

class ScriptAssertFailedException : public std::exception
{
public:
	ScriptAssertFailedException(const std::string& message);
	~ScriptAssertFailedException() override = default;

	const char* what() const noexcept override;

private:
	std::string message;
};
