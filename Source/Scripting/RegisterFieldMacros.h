#pragma once

// The parameter name must be the exact name of the field inside the class
#define REGISTER_FIELD(name, Type) \
    this->members.push_back(std::make_pair(TypeToEnum<Type>::value, Field<Type>( \
        #name, \
        [this] { return this->name; }, \
        [this](Type value) { this->name =value; } \
    )));

// The parameter Name must be one such that Get{Name} and Set{Name} functions exist as members of the class
#define REGISTER_FIELD_WITH_ACCESSORS(Name, Type) \
    this->members.push_back(std::make_pair(TypeToEnum<Type>::value, Field<Type>( \
        #Name, \
        [this] { return this->Get##Name(); }, \
        [this](Type value) { this->Set##Name(value); } \
    )));

#define REGISTER_VECTOR_WITH_ACCESSORS(Name, Type) \
    this->members.push_back(std::make_pair(FieldType::VECTOR, VectorField( \
        #Name, \
        [this] \
		{ \
			return TypeToAny<Type>(this->Get##Name()); \
		}, \
        [this](const std::vector<std::any>& value) \
		{ \
			this->Set##Name(AnyToType<Type>(value)); \
		}, \
		TypeToEnum<Type>::value \
    )));

namespace
{
	template<typename T>
	std::vector<std::any> TypeToAny(const std::vector<T>& vector)
	{
		std::vector<std::any> convertedVector;
		convertedVector.reserve(vector.size());
		for (const T& element : vector)
		{
			convertedVector.push_back(element);
		}
		return convertedVector;
	}

	template<typename T>
	std::vector<T> AnyToType(const std::vector<std::any>& vector)
	{
		std::vector<T> convertedVector;
		convertedVector.reserve(vector.size());
		for (const std::any& element : vector)
		{
			convertedVector.push_back(std::any_cast<T>(element));
		}
		return convertedVector;
	}
}
