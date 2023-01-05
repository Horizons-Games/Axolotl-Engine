#pragma once
#include <list>
#include <MathGeoLib/Include/Geometry/AABB.h>
#include "Globals.h"

class GameObject;

class Quadtree
{
public:
	Quadtree(const AABB& boundingBox);
	~Quadtree();

	bool IsLeaf() const;
	void Add(GameObject* gameObject);
	void Remove(GameObject* gameObject);
	bool InQuadrant(GameObject* gameObject);
	void Subdivide(GameObject* gameObject);
	void DeleteGameObject(GameObject* gameObject);
	void Clear();

	const std::list<GameObject*>& GetIntersectingGameObjects(const float4x4& projectionMatrix);
	const std::list<GameObject*>& GetGameObjectsToDraw(const AABB& cameraAABB);

	void SetQuadrantCapacity(int quadrantCapacity);
	void SetMinCubeSize(float minCubeSize);

private:

	std::list<GameObject*> gameObjects;
	AABB boundingBox;

	int quadrantCapacity = QUADRANT_CAPACITY;
	float minQuadrantDiagonalSquared = 3 * MIN_CUBE_SIZE * MIN_CUBE_SIZE; // D^2 = 3C^2

	Quadtree* frontRightNode = nullptr;
	Quadtree* frontLeftNode = nullptr;
	Quadtree* backRightNode = nullptr;
	Quadtree* backLeftNode = nullptr;
};

