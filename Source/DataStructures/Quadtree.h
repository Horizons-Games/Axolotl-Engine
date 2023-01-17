#pragma once
#include <list>
#include <MathGeoLib/Include/Geometry/AABB.h>
#include "Globals.h"

class GameObject;

class Quadtree
{
public:
	Quadtree(const AABB& boundingBox);
	Quadtree(const AABB& boundingBox, Quadtree* root);
	~Quadtree();

	bool IsLeaf() const;
	bool InQuadrant(GameObject* gameObject);

	void Add(GameObject* gameObject);
	void AddGameObjectAndChildren(GameObject* gameObject);
	void Remove(GameObject* gameObject);
	void RemoveGameObjectAndChildren(GameObject* gameObject);
	void SmartRemove();
	void OptimizeParentObjects();
	
	void Subdivide();
	void RedistributeGameObjects(GameObject* gameobject);

	void ExpandToFit(GameObject* gameObject);
	void AdjustHeightToNodes(float minY, float maxY);

	void ResetChildren();

	const std::list<GameObject*>& GetGameObjects() const;
	void GetFamilyObjects(std::list<GameObject*>& familyGameObjects);

	Quadtree* GetFrontRightNode() const;
	Quadtree* GetFrontLeftNode() const;
	Quadtree* GetBackRightNode() const;
	Quadtree* GetBackLeftNode() const;

	bool IsFreezed() const;
	void SetFreezedStatus(bool isFreezed);

	int GetQuadrantCapacity() const;
	void SetQuadrantCapacity(int quadrantCapacity);

	float GetMinQuadrantSideSize() const;
	void SetMinQuadrantSideSize(float minCubeSize);

	const AABB& GetBoundingBox() const;
	void SetBoundingBox(AABB boundingBox);

	std::list<GameObject*> GetAllGameObjects(GameObject* gameObject);

private:

	std::list<GameObject*> gameObjects;
	AABB boundingBox;

	int quadrantCapacity = QUADRANT_CAPACITY;
	float minQuadrantSideSize = MIN_CUBE_SIZE;
	float minQuadrantDiagonalSquared = 3 * MIN_CUBE_SIZE * MIN_CUBE_SIZE; // D^2 = 3C^2

	Quadtree* parent;

	Quadtree* frontRightNode = nullptr;
	Quadtree* frontLeftNode = nullptr;
	Quadtree* backRightNode = nullptr;
	Quadtree* backLeftNode = nullptr;

	bool isFreezed = false;
};

inline bool Quadtree::IsFreezed() const
{
	return isFreezed;
}

inline void Quadtree::SetFreezedStatus(bool isFreezed)
{
	this->isFreezed = isFreezed;
}
 
inline const AABB& Quadtree::GetBoundingBox() const
{
	return boundingBox;
}

inline void Quadtree::SetBoundingBox(AABB boundingBox)
{
	this->boundingBox = boundingBox;
}

inline int Quadtree::GetQuadrantCapacity() const
{
	return quadrantCapacity;
}

inline float Quadtree::GetMinQuadrantSideSize() const
{
	return minQuadrantSideSize;
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

inline void Quadtree::SetQuadrantCapacity(int quadrantCapacity)
{
	this->quadrantCapacity = quadrantCapacity;
}

inline void Quadtree::SetMinQuadrantSideSize(float minQuadrantSideSize)
{
	this->minQuadrantSideSize = minQuadrantSideSize;
	minQuadrantDiagonalSquared = 3 * minQuadrantSideSize * minQuadrantSideSize;
}

