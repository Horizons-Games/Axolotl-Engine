#pragma once

enum class FieldType
{
	FLOAT,
	BOOL
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
