#include "ModuleScene.h"
#include "Quadtree.h"
#include "GameObject/GameObject.h"

ModuleScene::ModuleScene()
{}

ModuleScene::~ModuleScene()
{
	delete root;
	delete sceneQuadTree;
	root = nullptr;
}

bool ModuleScene::Init()
{
	root = new GameObject("Root");
	sceneQuadTree = new Quadtree(rootQuadtreeAABB);
	FillQuadtree(root); //TODO: This call has to be moved AFTER the scene is loaded
	return true;
}

void ModuleScene::FillQuadtree(GameObject* gameObject) 
{
	sceneQuadTree->Add(gameObject);
	if (!gameObject->GetChildren().empty())
	{
		for (GameObject* child : gameObject->GetChildren()) FillQuadtree(child);
	}
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
	//sceneQuadTree->Add(gameObject);
	return gameObject;
}

void ModuleScene::UpdateGameObjectAndDescendants(GameObject* gameObject)
{
	gameObject->Update();

	for (GameObject* child : gameObject->GetChildren())
		UpdateGameObjectAndDescendants(child);
}
