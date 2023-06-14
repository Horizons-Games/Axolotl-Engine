#include "ComponentCubemap.h"
#include "Components/Component.h"
#include "GameObject/GameObject.h"
#include "ModuleScene.h"
#include "Application.h"
#include "Scene/Scene.h"
#include "Cubemap/Cubemap.h"
#include "FileSystem/Json.h"

ComponentCubemap::ComponentCubemap(bool active, GameObject* owner) :
	Component(ComponentType::CUBEMAP, active, owner, true)
{
}

ComponentCubemap::~ComponentCubemap()
{
}

void ComponentCubemap::InternalSave(Json& meta)
{
	const ModuleScene* scene = App->GetModule<ModuleScene>();
	const Scene* loadedScene = scene->GetLoadedScene();
	Cubemap* currentCubemap = loadedScene->GetCubemap();

	meta["intensity"] = currentCubemap->GetIntensity();
}

void ComponentCubemap::InternalLoad(Json& meta)
{
	const ModuleScene* scene = App->GetModule<ModuleScene>();
	const Scene* loadedScene = scene->GetLoadedScene();
	Cubemap* currentCubemap = loadedScene->GetCubemap();
	currentCubemap->SetIntensity((float) meta["intensity"]);
}