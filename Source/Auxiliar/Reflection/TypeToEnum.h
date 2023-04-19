#pragma once

enum class FieldType
{
	FLOAT,
	STRING
};

template<typename T>
struct TypeToEnum
{
};

template<>
struct TypeToEnum<float>
{
	const static FieldType value = FieldType::FLOAT;
};

template<>
struct TypeToEnum<std::string>
{
	const static FieldType value = FieldType::STRING;
};