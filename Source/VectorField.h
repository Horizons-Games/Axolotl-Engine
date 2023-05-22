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
        Field<T>(name, getter, setter)
    {
        type = GetType();
    }

private:
    template<typename U>
    static FieldType GetTypeImpl()
    {
        if constexpr (std::is_same_v<U, float>)
            return FieldType::FLOAT;
        else if constexpr (std::is_same_v<U, std::string>)
            return FieldType::STRING;
        // More types here
        else
            return FieldType::UNKNOWN;
    }

    FieldType GetType()
    {
        try
        {
            T test;
            if (!test.empty())
            {
                using ElementType = typename T::value_type;
                return GetTypeImpl<ElementType>();
            }
        }
        catch (const std::bad_any_cast&)
        {
            return FieldType::UNKNOWN;
        }

        return FieldType::UNKNOWN;
    }
};
