#pragma once

#include <exception>

class AxoDefaultException : public std::exception
{
public:
	AxoDefaultException(const std::string& message);
	~AxoDefaultException() override = default;

	const char* what() const noexcept override;

private:
	std::string message;
};
