#include "ModuleScene.h"
#include "GameObject/GameObject.h"

#include <assert.h>

ModuleScene::ModuleScene()
{
}

ModuleScene::~ModuleScene()
{
	delete root;
	root = nullptr;
}

bool ModuleScene::Init()
{
	root = new GameObject("Scene Root");
	return true;
}

update_status ModuleScene::Update()
{
	UpdateGameObjectAndDescendants(root);

	return UPDATE_CONTINUE;
}

GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent)
{
	assert(name != nullptr && parent != nullptr);

	GameObject* gameObject = new GameObject(name, parent);

	return gameObject;
}

void ModuleScene::UpdateGameObjectAndDescendants(GameObject* gameObject)
{
	assert(gameObject != nullptr);

	gameObject->Update();

	for (GameObject* child : gameObject->GetChildren())
	{
		UpdateGameObjectAndDescendants(child);
	}
}
