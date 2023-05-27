#include "ComponentCubeMap.h"
#include "Components/Component.h"
#include "GameObject/GameObject.h"
#include "ModuleScene.h"
#include "Application.h"
#include "Scene/Scene.h"
#include "Cubemap/Cubemap.h"
#include "FileSystem/Json.h"

ComponentCubeMap::ComponentCubeMap(bool active, GameObject* owner) :
	Component(ComponentType::CUBEMAP, active, owner, true)
{
}

ComponentCubeMap::~ComponentCubeMap()
{
}

void ComponentCubeMap::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool) active;
	meta["removed"] = (bool) canBeRemoved;

	meta["intensity"] = (float) intensity;
}

void ComponentCubeMap::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool) meta["active"];
	canBeRemoved = (bool) meta["removed"];

	intensity = (bool) meta["intensity"];
}

inline void ComponentCubeMap::SetIntensity(float intensity)
{
	this->intensity = intensity;
	const ModuleScene* scene = App->GetModule<ModuleScene>();
	const Scene* loadedScene = scene->GetLoadedScene();
	Cubemap* currentCubeMap = loadedScene->GetCubemap();
	currentCubeMap->SetIntensity(intensity);
}
