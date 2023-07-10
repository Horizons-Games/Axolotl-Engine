#include "StdAfx.h"
#include "ComponentTrail.h"

#include "ComponentTransform.h"

#include "GameObject/GameObject.h"

#include "Trail/Trail.h"

ComponentTrail::ComponentTrail(bool active, GameObject* owner) : Component(ComponentType::TRAIL, active, owner, true)
{
}

ComponentTrail::~ComponentTrail()
{
}

void ComponentTrail::Draw() const
{
}

void ComponentTrail::Update()
{
	trail->Update(GetOwner()->GetComponent<ComponentTransform>()->GetGlobalPosition());
}
