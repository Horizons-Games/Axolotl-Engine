#include "ComponentAnimation.h"

ComponentAnimation::ComponentAnimation(const bool active, GameObject* owner)
	: Component(ComponentType::ANIMATION, active, owner, false)
{
}

ComponentAnimation::~ComponentAnimation()
{
}

void ComponentAnimation::Update()
{
	// Empty for now
}
