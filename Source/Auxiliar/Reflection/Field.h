#pragma once

#include <functional>
#include <string>

template<typename T>
struct Field
{
public:
	std::string name;
	std::function<T(void)> getter;
	std::function<void(const T&)> setter;

	Field(std::string&& name, std::function<T(void)>&& getter, std::function<void(const T&)>&& setter) :
		name(std::move(name)),
		getter(std::move(getter)),
		setter(std::move(setter))
	{
		if (!this->name.empty())
		{
			this->name.front() = std::toupper(this->name.front());
		}
	}

	virtual ~Field() = default;
};
