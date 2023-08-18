#include "StdAfx.h"

#include "ComponentLightProbe.h"
#include "GameObject/GameObject.h"
#include "ModuleScene.h"
#include "Application.h"
#include "Scene/Scene.h"
#include "Cubemap/Cubemap.h"
#include "FileSystem/Json.h"

ComponentLightProbe::ComponentLightProbe(bool active, GameObject* owner) :
	Component(ComponentType::LIGHT_PROBE, active, owner, true)
{
}

ComponentLightProbe::~ComponentLightProbe()
{
}

void ComponentLightProbe::InternalSave(Json& meta)
{
}

void ComponentLightProbe::InternalLoad(const Json& meta)
{
}