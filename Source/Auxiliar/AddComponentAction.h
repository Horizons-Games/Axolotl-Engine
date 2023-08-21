#pragma once

#include "GameObject/GameObject.h"

enum class ComponentFunctionality
{
	NONE,
	GRAPHICS,
	PHYSICS,
	GAMEPLAY,
	AUDIO
};

struct AddComponentAction
{
	std::string actionName;
	std::function<void(void)> callback;
	ComponentFunctionality functionalGroup;
	std::function<bool(GameObject*)> condition;

	AddComponentAction(std::string&& actionName,
					   std::function<void(void)>&& callback,
					   std::function<bool(GameObject*)>&& condition,
					   ComponentFunctionality functionalGroup) :
		actionName(std::move(actionName)),
		callback(std::move(callback)),
		condition(std::move(condition)),
		functionalGroup(functionalGroup)
	{
	}

	AddComponentAction(std::string&& actionName,
					   std::function<void(void)>&& callback,
					   ComponentFunctionality functionalGroup) :
		AddComponentAction(
			std::move(actionName),
			std::move(callback),
			[](GameObject*)
			{
				return true;
			},
			functionalGroup)
	{
	}

	friend bool operator<(const AddComponentAction& first, const AddComponentAction& second)
	{
		if (first.functionalGroup == second.functionalGroup)
		{
			return first.actionName < second.actionName;
		}
		return first.functionalGroup < second.functionalGroup;
	}
};