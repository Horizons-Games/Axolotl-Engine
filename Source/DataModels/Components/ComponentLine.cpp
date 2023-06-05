#include "ComponentLine.h"
#include"Components/ComponentTransform.h"
#include "FileSystem/Json.h"
#include "GameObject/GameObject.h"

ComponentLine::ComponentLine(const bool active, GameObject* owner) :
	Component(ComponentType::LINE, active, owner, true)
{
	childGameObject = new GameObject("Line End", owner);
	childGameObject->InitNewEmptyGameObject(true);
	ComponentTransform* transform =
		static_cast<ComponentTransform*>(childGameObject->GetComponent(ComponentType::TRANSFORM));
	transform->SetPosition(
		float3(transform->GetPosition().x + 1.0, transform->GetPosition().y , transform->GetPosition().z));
	transform->UpdateTransformMatrices();
}

ComponentLine::~ComponentLine()
{
}

void ComponentLine::Update()
{
}

void ComponentLine::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool) active;
	meta["removed"] = (bool) canBeRemoved;
}

void ComponentLine::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool) meta["active"];
	canBeRemoved = (bool) meta["removed"];
}