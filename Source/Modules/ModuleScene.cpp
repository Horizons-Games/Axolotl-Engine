#include "ModuleScene.h"
#include "GameObject/GameObject.h"

ModuleScene::ModuleScene()
{}

ModuleScene::~ModuleScene()
{
	delete root;
	root = nullptr;
}

bool ModuleScene::Init()
{
	root = new GameObject("Root");
	return true;
}

bool ModuleScene::IsInsideACamera(const OBB& obb)
{
	// TODO: We have to add all the cameras in the future
	return false;
}

update_status ModuleScene::Update()
{
	UpdateGameObjectAndDescendants(root);

	return UPDATE_CONTINUE;
}

GameObject* ModuleScene::CreateGameObject(const char* name, GameObject* parent)
{
	GameObject* gameObject = new GameObject(name, parent);

	return gameObject;
}

void ModuleScene::UpdateGameObjectAndDescendants(GameObject* gameObject)
{
	gameObject->Update();

	for (GameObject* child : gameObject->GetChildren())
		UpdateGameObjectAndDescendants(child);
}
