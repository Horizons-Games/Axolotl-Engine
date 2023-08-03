#include "StdAfx.h"

#include "ComponentSkybox.h"

ComponentSkybox::ComponentSkybox(bool active, GameObject* owner) : Component(ComponentType::SKYBOX, active, owner, true)
{
}

ComponentSkybox::~ComponentSkybox()
{
}

void ComponentSkybox::InternalSave(Json& meta)
{
	/*const ModuleScene* scene = App->GetModule<ModuleScene>();
	const Scene* loadedScene = scene->GetLoadedScene();
	Cubemap* currentCubemap = loadedScene->GetCubemap();

	meta["intensity"] = currentCubemap->GetIntensity();*/
}

void ComponentSkybox::InternalLoad(const Json& meta)
{
	//const ModuleScene* scene = App->GetModule<ModuleScene>();
	//const Scene* loadedScene = scene->GetLoadedScene();
	//Cubemap* currentCubemap = loadedScene->GetCubemap();
	//currentCubemap->SetIntensity((float) meta["intensity"]);
}