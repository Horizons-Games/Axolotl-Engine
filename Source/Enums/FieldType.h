#pragma once

#include "GameObject/GameObject.h"

enum class FieldType
{
	FLOAT,
	STRING,
	GAMEOBJECT,
	VECTOR3,
	BOOLEAN
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
struct TypeToEnum<float3>
{
	const static FieldType value = FieldType::VECTOR3;
};

template<>
struct TypeToEnum<std::string>
{
	const static FieldType value = FieldType::STRING;
};

template<>
struct TypeToEnum<GameObject*>
{
	const static FieldType value = FieldType::GAMEOBJECT;
};

template<>
struct TypeToEnum<bool>
{
	const static FieldType value = FieldType::BOOLEAN;
};
