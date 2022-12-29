#pragma once
#include <list>
#include <MathGeoLib/Include/Geometry/AABB.h>

class GameObject;

class Quadtree
{
public:
	Quadtree(const AABB& boundaryBox);
	~Quadtree();

	bool IsLeaf() const;
	bool Add(GameObject* gameObject);
	void Remove(GameObject* gameObject);
	bool InQuadrant(GameObject* gameObject);
	void Subdivide();
	void Clear();

private:
	int capacity;
	std::list<GameObject*> gameObjects;
	AABB boundaryBox;

	Quadtree* frontRightNode = nullptr;
	Quadtree* frontLeftNode = nullptr;
	Quadtree* backRightNode = nullptr;
	Quadtree* backLeftNode = nullptr;
};

