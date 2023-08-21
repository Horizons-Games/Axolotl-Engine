#pragma once
#include "Auxiliar/Reflection/Field.h"
#include "Enums/FieldType.h"

struct VectorField : public Field<std::vector<std::any>>
{
public:
	FieldType innerType;

	VectorField(std::string&& name,
		std::function<std::vector<std::any>(void)>&& getter,
		std::function<void(const std::vector<std::any>&)>&& setter,
		FieldType innerType) :
		Field<std::vector<std::any>>(std::move(name), std::move(getter), std::move(setter)),
		innerType(innerType)
	{
	}

	~VectorField() override = default;
};
