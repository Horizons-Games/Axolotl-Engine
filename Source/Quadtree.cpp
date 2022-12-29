#include "Quadtree.h"
#include "GameObject/GameObject.h"

Quadtree::Quadtree(const AABB& boundaryBox) : boundaryBox(boundaryBox)
{
	
}

Quadtree::~Quadtree()
{
}

bool Quadtree::IsLeaf() const
{
	return frontLeftNode == nullptr;
}

void Quadtree::Add(GameObject* gameObject)
{
	assert(gameObject != nullptr);
	assert(InQuadrant(gameObject));

	if (IsLeaf()) {
		if (gameObjects.size() < quadrantCapacity) gameObjects.push_back(gameObject);
		else if (boundaryBox.Diagonal().LengthSq() <= minQuadrantDiagonalSquared) gameObjects.push_back(gameObject);
		else Subdivide();
	}
	else 
	{
		frontLeftNode->Add(gameObject);
		frontRightNode->Add(gameObject);
		backLeftNode->Add(gameObject);
		backRightNode->Add(gameObject);
	}
}

void Quadtree::Remove(GameObject* gameObject)
{
}

bool Quadtree::InQuadrant(GameObject* gameObject)
{
	return false;
	//return !boundaryBox.Intersects(gameObject->GetAABB());
}

void Quadtree::Subdivide()
{
}

void Quadtree::Clear()
{
}
