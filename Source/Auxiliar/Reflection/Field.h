#pragma once

#include <string>
#include <functional>

enum class FieldType
{
	FLOAT
};

template<typename T>
struct Field
{
public:
	const std::string name;
	const std::function<T(void)> getter;
	const std::function<void(const T&)> setter;

	Field(const std::string& name,
		  const std::function<T(void)>& getter,
		  const std::function<void(const T&)>& setter) :
		name(name),
		getter(getter),
		setter(setter)
	{
	}

	~Field() = default;
};
