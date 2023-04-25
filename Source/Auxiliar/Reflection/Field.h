#pragma once

#include <functional>
#include <string>

template<typename T>
struct Field
{
public:
	std::string name;
	std::function<T(void)> getter;
	std::function<void(T)> setter;

	Field(const std::string& name, const std::function<T(void)>& getter, const std::function<void(T)>& setter) :
		name(name),
		getter(getter),
		setter(setter)
	{
	}

	~Field() = default;
};
