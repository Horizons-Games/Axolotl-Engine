#pragma once

#include "DataModels/Components/Component.h"

class DrawableComponent : public Component
{
public:
	DrawableComponent(const ComponentType type, const bool active, GameObject* owner, const bool canBeRemoved);
	virtual ~DrawableComponent() override;

	//this method should be const, but other Draws seem to have side effects, which shouldn't be the case
	virtual void Draw() = 0;
};

