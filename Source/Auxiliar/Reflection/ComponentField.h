#pragma once
#include "Auxiliar/Reflection/Field.h"
#include "Enums/ComponentType.h"

class Component;

class ComponentField : Field<Component*>
{
public:
	ComponentType actualType;

	ComponentField(const std::string& name,
				   const std::function<Component*(void)>& getter,
				   const std::function<void(Component*)>& setter,
				   ComponentType actualType) :
		Field<Component*>(name, getter, setter),
		actualType(actualType)
	{
	}
};
