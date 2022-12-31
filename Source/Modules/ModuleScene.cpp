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
	root->AddChild(new GameObject("aaaaaaaaa", root));
	root->AddChild(new GameObject("aaaaaaaaa", root));
	root->AddChild(new GameObject("aaaaaaaaa", root));
	root->AddChild(new GameObject("aaaaaaaaa", root));

	root->GetChildren()[2]->AddChild(new GameObject("bbbbbbbb", root->GetChildren()[2]));
	root->GetChildren()[2]->AddChild(new GameObject("bbbbbbbb", root->GetChildren()[2]));
	root->GetChildren()[2]->AddChild(new GameObject("bbbbbbbb", root->GetChildren()[2]));
	root->GetChildren()[2]->Disable();

	root->GetChildren()[2]->GetChildren()[1]->AddChild(new GameObject("ccccccccc", root->GetChildren()[2]->GetChildren()[1]));
	root->GetChildren()[2]->GetChildren()[0]->AddChild(new GameObject("ccccccccc", root->GetChildren()[2]->GetChildren()[0]));

	return true;
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
