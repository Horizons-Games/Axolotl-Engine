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
	void Draw();

	const std::list<GameObject*>& GetGameObjectsToDraw();
	const AABB& GetBoundingBox() const;
	const std::list<GameObject*>& GetGameObjects() const;
	Quadtree* GetFrontRightNode() const;
	Quadtree* GetFrontLeftNode() const;
	Quadtree* GetBackRightNode() const;
	Quadtree* GetBackLeftNode() const;
	int GetQuadrantCapacity() const;
	float GetQuadrantSize() const;

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

inline const AABB& Quadtree::GetBoundingBox() const
{
	return boundingBox;
}

inline int Quadtree::GetQuadrantCapacity() const
{
	return quadrantCapacity;
}

inline float Quadtree::GetQuadrantSize() const
{
	return sqrt(minQuadrantDiagonalSquared/3.0f);
}

inline const std::list<GameObject*>& Quadtree::GetGameObjects() const
{
	return gameObjects;
}

inline Quadtree* Quadtree::GetFrontRightNode() const
{
	return frontRightNode;
}

inline Quadtree* Quadtree::GetFrontLeftNode() const
{
	return frontLeftNode;
}

inline Quadtree* Quadtree::GetBackRightNode() const
{
	return backRightNode;
}

inline Quadtree* Quadtree::GetBackLeftNode() const
{
	return backLeftNode;
}

