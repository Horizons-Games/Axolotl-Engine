#include "StdAfx.h"

#include "ComponentMockState.h"
#include "ComponentTransform.h"

#include "Application.h"
#include "GameObject/GameObject.h"

#include "FileSystem/Json.h"

#include "Geometry/LineSegment.h"
#include "Geometry/Ray.h"
#include "Physics/Physics.h"

ComponentMockState::ComponentMockState(bool active, GameObject* owner) :
	Component(ComponentType::MOCKSTATE, active, owner, true),
	isWinState(false),
	isFailState(false)
{
}

ComponentMockState::~ComponentMockState()
{
}

void ComponentMockState::InternalSave(Json& meta)
{
	meta["isWinState"] = (bool) GetIsWinState();
	meta["isFailState"] = (bool) GetIsFailState();
	meta["sceneName"] = GetSceneName().c_str();
}

void ComponentMockState::InternalLoad(const Json& meta)
{
	std::string tag = meta["sceneName"];

	SetIsWinState((bool) meta["isWinState"]);
	SetIsFailState((bool) meta["isFailState"]);
	SetSceneName(tag.c_str());
}
