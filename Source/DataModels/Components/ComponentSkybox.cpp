#include "StdAfx.h"

#include "ComponentSkybox.h"
#include "GameObject/GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "Resources/ResourceSkyBox.h"
#include "Skybox/Skybox.h"
#include "Cubemap/Cubemap.h"

ComponentSkybox::ComponentSkybox(bool active, GameObject* owner) : 
	Component(ComponentType::SKYBOX, active, owner, true), useCubemap(false)
{ 
}

ComponentSkybox::~ComponentSkybox()
{
}

void ComponentSkybox::InternalSave(Json& meta)
{
	meta["useCubemap"] = useCubemap;
}

void ComponentSkybox::InternalLoad(const Json& meta)
{
	useCubemap = meta["useCubemap"];
}

void ComponentSkybox::SignalEnable()
{
	skybox->GetSkyboxResource()->Load();
}

void ComponentSkybox::SignalDisable()
{
	skybox->GetSkyboxResource()->Unload();
}

void ComponentSkybox::SetSkybox(Skybox* skybox)
{
	this->skybox = skybox;
	skybox->GetSkyboxResource()->Load();
	cubemap = new Cubemap(skybox->GetSkyboxResource()->GetGlTexture());
}

Skybox* ComponentSkybox::GetSkybox()
{
	return skybox;
}

Cubemap* ComponentSkybox::GetCubemap()
{
	return cubemap;
}
