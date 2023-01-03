#include "ModuleScene.h"

GameObject* ModuleScene::CreateGameObject()
{
    return nullptr;
}

GameObject* ModuleScene::GetGameObject(UID id) const
{
	if (game_objects_id_map.count(id) == 0) return nullptr;

	return game_objects_id_map.at(id);
}
