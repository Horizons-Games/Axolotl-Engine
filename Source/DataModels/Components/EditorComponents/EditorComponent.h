#pragma once

#include "DataModels/Components/Component.h"

template<typename C = Component>
class EditorComponent : public C
{
public:
	EditorComponent(const ComponentType type, const bool active, GameObject* owner, const bool canBeRemoved);
	virtual ~EditorComponent() override;
};

template<typename C>
EditorComponent<C>::EditorComponent(const ComponentType type,
									const bool active,
									GameObject* owner,
									const bool canBeRemoved) : 
	C(type, active, owner, canBeRemoved)
{
}

template<typename C>
EditorComponent<C>::~EditorComponent()
{
}
