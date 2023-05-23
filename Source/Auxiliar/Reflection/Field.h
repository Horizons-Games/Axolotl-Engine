#pragma once

#include <string>
#include <functional>

template<typename T>
struct Field
{
public:
    std::string name;
    std::function<T(void)> getter;
    std::function<void(const T&)> setter;

    Field(const std::string& name,
        const std::function<T(void)>& getter,
        const std::function<void(const T&)>& setter) :
        name(name),
        getter(getter),
        setter(setter)
    {
        if (!name.empty())
        {
            this->name.front() = std::toupper(this->name.front());
        }
    }

	~Field() = default;
};
