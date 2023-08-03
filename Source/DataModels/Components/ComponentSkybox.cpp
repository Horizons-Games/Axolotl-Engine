#include "StdAfx.h"

#include "ComponentSkybox.h"
#include "GameObject/GameObject.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "Resources/ResourceSkyBox.h"

ComponentSkybox::ComponentSkybox(bool active, GameObject* owner) : 
	Component(ComponentType::SKYBOX, active, owner, true)
{ 
}

ComponentSkybox::~ComponentSkybox()
{
}

void ComponentSkybox::InternalSave(Json& meta)
{
}

void ComponentSkybox::InternalLoad(const Json& meta)
{
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
}

Skybox* ComponentSkybox::GetSkybox()
{
	return skybox;
}
