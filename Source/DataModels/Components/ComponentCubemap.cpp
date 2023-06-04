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

void ComponentCubemap::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool) active;
	meta["removed"] = (bool) canBeRemoved;

	const ModuleScene* scene = App->GetModule<ModuleScene>();
	const Scene* loadedScene = scene->GetLoadedScene();
	Cubemap* currentCubemap = loadedScene->GetCubemap();

	meta["intensity"] = currentCubemap->GetIntensity();
	
}

void ComponentCubemap::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool) meta["active"];
	canBeRemoved = (bool) meta["removed"];

	const ModuleScene* scene = App->GetModule<ModuleScene>();
	const Scene* loadedScene = scene->GetLoadedScene();
	Cubemap* currentCubemap = loadedScene->GetCubemap();
	currentCubemap->SetIntensity((float) meta["intensity"]);
}