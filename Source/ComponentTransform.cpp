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
	// TODO: Change position/rotation/scale to match user input
}