#pragma once
#include <list>
#include <set>
#include <map>
#include <MathGeoLib/Include/Geometry/AABB.h>

#include "Globals.h"
#include "Geometry/LineSegment.h"

class GameObject;
class Json;

class Quadtree
{
public:
	Quadtree(const AABB& boundingBox);
	Quadtree(const AABB& boundingBox, Quadtree* root);
	~Quadtree();

	bool IsLeaf() const;
	bool InQuadrant(const GameObject* gameObject) const;
	bool EntireInQuadrant(const GameObject* gameObject) const;

	void Add(GameObject* gameObject);
	void AddGameObjectAndChildren(GameObject* gameObject);
	bool Remove(const GameObject* gameObject);
	void RemoveGameObjectAndChildren(const GameObject* gameObject);
	bool SmartRemove();

	void Subdivide();
	void RedistributeGameObjects(GameObject* gameObject);

	void ExpandToFit(GameObject* gameObject);
	void AdjustHeightToNodes(float minY, float maxY);

	void ResetChildren();

	const std::set<GameObject*>& GetGameObjects() const;
	void GetFamilyObjects(std::set<GameObject*>& familyGameObjects);

	void SaveOptions(Json& meta);
	void LoadOptions(Json& meta);

	const Quadtree* GetFrontRightNode() const;
	const Quadtree* GetFrontLeftNode() const;
	const Quadtree* GetBackRightNode() const;
	const Quadtree* GetBackLeftNode() const;

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
	std::set<GameObject*> gameObjects;
	AABB boundingBox;

	int quadrantCapacity;
	float minQuadrantSideSize;
	float minQuadrantDiagonalSquared;

	Quadtree* parent;

	std::unique_ptr<Quadtree> frontRightNode;
	std::unique_ptr<Quadtree> frontLeftNode;
	std::unique_ptr<Quadtree> backRightNode;
	std::unique_ptr<Quadtree> backLeftNode;

	bool isFreezed;
};

inline bool Quadtree::IsLeaf() const
{
	return frontLeftNode == nullptr;
}

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

inline const std::set<GameObject*>& Quadtree::GetGameObjects() const
{
	return gameObjects;
}

inline const Quadtree* Quadtree::GetFrontRightNode() const
{
	return frontRightNode.get();
}

inline const Quadtree* Quadtree::GetFrontLeftNode() const
{
	return frontLeftNode.get();
}

inline const Quadtree* Quadtree::GetBackRightNode() const
{
	return backRightNode.get();
}

inline const Quadtree* Quadtree::GetBackLeftNode() const
{
	return backLeftNode.get();
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

