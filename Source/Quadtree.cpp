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

	if (IsLeaf()) 
	{
		if (gameObjects.size() < quadrantCapacity) gameObjects.push_back(gameObject);
		else if (boundaryBox.Diagonal().LengthSq() <= minQuadrantDiagonalSquared) gameObjects.push_back(gameObject);
		else Subdivide(gameObject);
	}
	else 
	{
		frontRightNode->Add(gameObject);
		frontLeftNode->Add(gameObject);
		backRightNode->Add(gameObject);
		backLeftNode->Add(gameObject);
	}
}

void Quadtree::Remove(GameObject* gameObject)
{
	if (!IsLeaf())
	{
		frontRightNode->Remove(gameObject);
		frontLeftNode->Remove(gameObject);
		backRightNode->Remove(gameObject);
		backLeftNode->Remove(gameObject);
	}

	std::list<GameObject*>::iterator it = std::find(gameObjects.begin(), gameObjects.end(), gameObject);
	if (it != gameObjects.end())
		gameObjects.erase(it);

	
}

bool Quadtree::InQuadrant(GameObject* gameObject)
{
	return true;
	//return !boundaryBox.Intersects(gameObject->GetAABB());
}

void Quadtree::Subdivide(GameObject* gameObject)
{
	float3 currentSize = boundaryBox.Size();
	float xSize = currentSize.x;
	float zSize = currentSize.z;

	float3 newCenterFrontRight = boundaryBox.CenterPoint() + vec(xSize * 0.25f, 0.0f, zSize * 0.25f);
	float3 newCenterFrontLeft = boundaryBox.CenterPoint() + vec(-xSize * 0.25f, 0.0f, zSize * 0.25f);
	float3 newCenterBackRight = boundaryBox.CenterPoint() + vec(xSize * 0.25f, 0.0f, -zSize * 0.25f);
	float3 newCenterBackLeft = boundaryBox.CenterPoint() + vec(-xSize * 0.25f, 0.0f, -zSize * 0.25f);

	AABB quadrantBoundaryBox;
	float3 newSize(xSize * 0.5f, currentSize.y, zSize * 0.5f);
	
	quadrantBoundaryBox.SetFromCenterAndSize(newCenterFrontRight, newSize);
	frontRightNode = new Quadtree(quadrantBoundaryBox);
	
	quadrantBoundaryBox.SetFromCenterAndSize(newCenterFrontLeft, newSize);
	frontLeftNode = new Quadtree(quadrantBoundaryBox);

	quadrantBoundaryBox.SetFromCenterAndSize(newCenterBackRight, newSize);
	backRightNode = new Quadtree(quadrantBoundaryBox);

	quadrantBoundaryBox.SetFromCenterAndSize(newCenterBackLeft, newSize);
	backLeftNode = new Quadtree(quadrantBoundaryBox);

	gameObjects.push_back(gameObject);

	for (std::list<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end();)
	{
		GameObject* go = *it;

		bool inFrontRight = frontRightNode->InQuadrant(go);
		bool inFrontLeft = frontLeftNode->InQuadrant(go);
		bool inBackRight = backRightNode->InQuadrant(go);
		bool inBackLeft = backLeftNode->InQuadrant(go);

		if (inFrontRight && inFrontLeft && inBackRight && inBackLeft) ++it;
		else
		{
			it = gameObjects.erase(it);
			if (inFrontRight) frontRightNode->Add(go);
			if (inFrontLeft) frontLeftNode->Add(go);
			if (inBackRight) backRightNode->Add(go);
			if (inBackLeft) backLeftNode->Add(go);
		}
	}
}

void Quadtree::Clear()
{
}
