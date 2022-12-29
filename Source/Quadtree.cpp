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

bool Quadtree::Add(GameObject* gameObject)
{
	if (InQuadrant(gameObject)) return false;

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
