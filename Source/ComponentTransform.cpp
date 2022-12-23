#include "ComponentTransform.h"

ComponentTransform::ComponentTransform(const bool active, GameObject* owner)
	: Component(ComponentType::TRANSFORM, active, owner)
{
}

ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::Update()
{
	// TODO: Perform Transform updates (move, rotate, scale...)
}