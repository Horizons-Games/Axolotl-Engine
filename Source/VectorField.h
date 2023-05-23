#pragma once

#include <vector>
#include <any>
#include <type_traits>

template<typename T>
struct VectorField : public Field<T>
{
public:
    FieldType type;

    VectorField(const std::string& name,
        const std::function<T(void)>& getter,
        const std::function<void(const T&)>& setter) :
        Field<T>(name, getter, setter),
        type(GetType())
    {
    }

private:
    FieldType GetType()
    {
        if constexpr (std::is_same_v<typename T::value_type, float>)
            return FieldType::FLOAT;
        else if constexpr (std::is_same_v<typename T::value_type, std::string>)
            return FieldType::STRING;
        // More types here
        else
            return FieldType::UNKNOWN;
    }
};
