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
        const std::function<std::vector<T>(void)>& getter,
        const std::function<void(const std::vector<T>&)>& setter) :
        Field<T>(name, getter, setter),
        type(GetType())
    {
    }

private:
    FieldType GetType()
    {
        return TypeToEnum<T>::value;
    }
};
