#pragma once
#include "Auxiliar/Reflection/Field.h"
#include "Enums/FieldType.h"

#include <vector>
#include <any>

struct VectorField : public Field<std::vector<std::any>>
{
public:
	FieldType innerType;

	VectorField(const std::string& name,
		const std::function<std::vector<std::any>(void)>& getter,
		const std::function<void(const std::vector<std::any>&)>& setter,
		FieldType innerType) :
		Field<std::vector<std::any>>(name, getter, setter),
		innerType(innerType)
	{
	}
};
