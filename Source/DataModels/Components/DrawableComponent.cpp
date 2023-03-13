#include "DrawableComponent.h"

DrawableComponent::DrawableComponent(const ComponentType type,
									 const bool active,
									 GameObject* owner,
									 const bool canBeRemoved) : Component(type, active, owner, canBeRemoved)
{
}

DrawableComponent::~DrawableComponent()
{
}
