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
	void Clear();

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

