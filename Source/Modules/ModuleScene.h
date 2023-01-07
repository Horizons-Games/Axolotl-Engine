#pragma once

#include "Module.h"
#include "Geometry/OBB.h" 
#include "Geometry/AABB.h" 

class GameObject;
class Quadtree;

class ModuleScene :public Module
{
public:
	ModuleScene();
	~ModuleScene();

	bool Init() override;
	update_status Update() override;

	void FillQuadtree(GameObject* gameObject);
	bool IsInsideACamera(const OBB& obb);
	GameObject* CreateGameObject(const char* name, GameObject* parent);
	Quadtree* GetSceneQuadTree() const;

private:
	void UpdateGameObjectAndDescendants(GameObject* gameObject);

private:
	GameObject* root = nullptr;
	AABB rootQuadtreeAABB = AABB(float3(-100, 0, -100), float3(100, 50, 100));
	Quadtree* sceneQuadTree = nullptr;
};

inline Quadtree* ModuleScene::GetSceneQuadTree() const
{
	return sceneQuadTree;
}

