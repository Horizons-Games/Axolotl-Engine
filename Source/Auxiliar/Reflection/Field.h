#pragma once

#include <string>
#include <functional>

template<typename T>
struct Field
{
public:
	const std::string name;
	const std::function<T(void)> getter;
	const std::function<void(const T&)> setter;
	FieldType type;

	Field(const std::string& name,
		  const std::function<T(void)>& getter,
		  const std::function<void(const T&)>& setter,
		  FieldType type) :
		name(name),
		getter(getter),
		setter(setter),
		type(type)
	{
	}

	~Field() = default;
};
