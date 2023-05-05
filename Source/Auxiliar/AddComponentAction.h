#pragma once

#include <functional>

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
	std::function<bool(GameObject*)> condition;
	ComponentFunctionality functionalGroup;

	AddComponentAction(const std::string& actionName,
		const std::function<void(void)>& callback,
		const std::function<bool(GameObject*)>& condition,
		ComponentFunctionality functionalGroup)
		: actionName(actionName), callback(callback), condition(condition), functionalGroup(functionalGroup)
	{
	}

	AddComponentAction(const std::string& actionName,
		const std::function<void(void)>& callback,
		const std::function<bool(GameObject*)>& condition)
		: AddComponentAction(actionName, callback, condition, ComponentFunctionality::NONE)
	{
	}

	AddComponentAction(const std::string& actionName,
		const std::function<void(void)>& callback,
		ComponentFunctionality functionalGroup)
		: AddComponentAction(actionName, callback, [](GameObject*) { return true; }, functionalGroup)
	{
	}

	AddComponentAction(const std::string& actionName, const std::function<void(void)>& callback)
		: AddComponentAction(actionName, callback, [](GameObject*) { return true; }, ComponentFunctionality::NONE)
	{
	}

	friend bool operator< (const AddComponentAction& first, const AddComponentAction& second)
	{
		if (first.functionalGroup == second.functionalGroup)
		{
			return first.actionName < second.actionName;
		}
		return first.functionalGroup < second.functionalGroup;
	}
};