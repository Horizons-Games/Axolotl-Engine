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
	ComponentTransform* trans = GetOwner()->GetComponent<ComponentTransform>();
	trail->Update(trans->GetGlobalPosition(), trans->GetGlobalRotation());
}

void ComponentTrail::InternalSave(Json& meta)
{
}

void ComponentTrail::InternalLoad(const Json& meta)
{
}
