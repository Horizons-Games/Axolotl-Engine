#include "StdAfx.h"

#include "ComponentRender.h"

#include "FileSystem/Json.h"

#include "Application.h"

ComponentRender::ComponentRender(bool active, GameObject* owner) : 
	Component(ComponentType::RENDER, active, owner, false)
{
	moduleRender = App->GetModule<ModuleRender>();
}

ComponentRender::~ComponentRender()
{
}

void ComponentRender::InternalSave(Json& meta)
{
	meta["bloom_intensity"] = moduleRender->GetBloomIntensity();
}

void ComponentRender::InternalLoad(const Json& meta)
{
	moduleRender->SetBloomIntensity(static_cast<float>(meta["bloom_intensity"]));
}
