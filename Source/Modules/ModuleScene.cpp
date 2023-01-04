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
	selectedGameObject = root;

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

GameObject* ModuleScene::SearchGameObjectByID(UID gameObjectID) const
{
	return SearchGameObjectByIDRecursive(root, gameObjectID);
}

GameObject* ModuleScene::SearchGameObjectByIDRecursive(GameObject* gameObject, UID gameObjectID) const
{
	if (gameObject->GetUID() == gameObjectID)
	{
		return gameObject;
	}

	if (gameObject->GetChildren().empty())
	{
		assert(false && "Wrong GameObjectID introduced, GameObject not found");

		return nullptr;
	}

	for (GameObject* child : gameObject->GetChildren())
	{
		if (IsInThisBranch(child, gameObjectID))
		{
			return SearchGameObjectByIDRecursive(child, gameObjectID);
		}
	}
}

bool ModuleScene::IsInThisBranch(GameObject* gameObject, UID gameObjectID) const
{
	if (gameObject->GetUID() == gameObjectID)
	{
		return true;
	}

	if (gameObject->GetChildren().empty())
	{
		return false;
	}

	for (GameObject* child : gameObject->GetChildren())
	{
		if (IsInThisBranch(child, gameObjectID))
		{
			return true;
		}
	}

	return false;
}
