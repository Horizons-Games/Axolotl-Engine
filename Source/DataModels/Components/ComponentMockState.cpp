#include "ComponentMockState.h"
#include "ComponentTransform.h"

#include "GameObject/GameObject.h"
#include "Application.h"

#include "FileSystem/Json.h"

#include "Geometry/LineSegment.h"
#include "Geometry/Ray.h"
#include "Physics/Physics.h"

ComponentMockState::ComponentMockState(bool active, GameObject* owner)
	: Component(ComponentType::MOCKSTATE, active, owner, true)
{
	isWinState = false;
	isFailState = false;
}

ComponentMockState::~ComponentMockState()
{
}

void ComponentMockState::Update()
{
}

void ComponentMockState::Draw()
{

}

void ComponentMockState::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;
	meta["isWinState"] = (bool)GetIsWinState();
	meta["isFailState"] = (bool)GetIsFailState();
}

void ComponentMockState::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];
	SetIsWinState((bool)meta["isWinState"]);
	SetIsFailState((bool)meta["isFailState"]);
}