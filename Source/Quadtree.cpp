#include "Quadtree.h"
#include "GameObject/GameObject.h"
#include "Application.h"

#include "math/float4x4.h"
#include "geometry/OBB.h"
#include "geometry/AABB.h"

#include "ModuleEngineCamera.h"
#include "ModuleScene.h"
#include "Scene.h"


Quadtree::Quadtree(const AABB& boundingBox) : boundingBox(boundingBox)
{
}

Quadtree::~Quadtree()
{
	delete frontLeftNode;
	delete frontRightNode;
	delete backLeftNode;
	delete backRightNode;

	/*
	for (std::list<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
	{
		delete* it;
	}
	*/
	

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
		else if (boundingBox.Diagonal().LengthSq() <= minQuadrantDiagonalSquared) gameObjects.push_back(gameObject);
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
	std::list<GameObject*>::iterator it = std::find(gameObjects.begin(), gameObjects.end(), gameObject);
	if (it != gameObjects.end()) gameObjects.erase(it);

	if (!IsLeaf())
	{
		frontRightNode->Remove(gameObject);
		frontLeftNode->Remove(gameObject);
		backRightNode->Remove(gameObject);
		backLeftNode->Remove(gameObject);
	}
}

bool Quadtree::InQuadrant(GameObject* gameObject)
{
	//return boundingBox.Intersects(gameObject->GetAABB());
	//Dummy implementation until we can get the AABB from mesh
	return true;
}

void Quadtree::Subdivide(GameObject* gameObject)
{

	// Subdivision part
	float3 currentSize = boundingBox.Size();
	float xSize = currentSize.x;
	float zSize = currentSize.z;

	float3 currentCenterPoint = boundingBox.CenterPoint();

	float3 newCenterFrontRight = currentCenterPoint + vec(xSize * 0.25f, 0.0f, zSize * 0.25f);
	float3 newCenterFrontLeft = currentCenterPoint + vec(-xSize * 0.25f, 0.0f, zSize * 0.25f);
	float3 newCenterBackRight = currentCenterPoint + vec(xSize * 0.25f, 0.0f, -zSize * 0.25f);
	float3 newCenterBackLeft = currentCenterPoint + vec(-xSize * 0.25f, 0.0f, -zSize * 0.25f);

	AABB quadrantBoundingBox;
	float3 newSize(xSize * 0.5f, currentSize.y, zSize * 0.5f);
	
	quadrantBoundingBox.SetFromCenterAndSize(newCenterFrontRight, newSize);
	frontRightNode = new Quadtree(quadrantBoundingBox);
	
	quadrantBoundingBox.SetFromCenterAndSize(newCenterFrontLeft, newSize);
	frontLeftNode = new Quadtree(quadrantBoundingBox);

	quadrantBoundingBox.SetFromCenterAndSize(newCenterBackRight, newSize);
	backRightNode = new Quadtree(quadrantBoundingBox);

	quadrantBoundingBox.SetFromCenterAndSize(newCenterBackLeft, newSize);
	backLeftNode = new Quadtree(quadrantBoundingBox);

	// GameObject redistribution part
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
	this->gameObjects.clear();
	if (!IsLeaf())
	{
		this->frontRightNode->Clear();
		this->frontRightNode->Clear();
		this->frontRightNode->Clear();
		this->frontRightNode->Clear();
	}
}

// Draw recursively in the scene
void Quadtree::Draw()
{
	if (App->engineCamera->IsInside(boundingBox) || App->scene->GetLoadedScene()->IsInsideACamera(boundingBox))
	{
		for (GameObject* gameObject : gameObjects) gameObject->Draw();
		if (!IsLeaf())
		{
			this->frontRightNode->Draw();
			this->frontRightNode->Draw();
			this->frontRightNode->Draw();
			this->frontRightNode->Draw();
		}

	}
}

//One alternative approach for selecting the GameObjects to Draw
const std::list<GameObject*>& Quadtree::GetGameObjectsToDraw()
{
	std::list<GameObject*> intersectingGameObjects;
	std::list<GameObject*> auxGameObjects;
	//TODO replace with camera component function
	if (true)
	{
		intersectingGameObjects.insert(std::end(intersectingGameObjects), std::begin(this->gameObjects), std::end(this->gameObjects));
		if (!IsLeaf())
		{
			auxGameObjects = this->frontLeftNode->GetGameObjectsToDraw();
			intersectingGameObjects.insert(std::end(intersectingGameObjects), std::begin(auxGameObjects), std::end(auxGameObjects));
			auxGameObjects.clear();

			auxGameObjects = this->frontRightNode->GetGameObjectsToDraw();
			intersectingGameObjects.insert(std::end(intersectingGameObjects), std::begin(auxGameObjects), std::end(auxGameObjects));
			auxGameObjects.clear();

			auxGameObjects = this->backLeftNode->GetGameObjectsToDraw();
			intersectingGameObjects.insert(std::end(intersectingGameObjects), std::begin(auxGameObjects), std::end(auxGameObjects));
			auxGameObjects.clear();

			auxGameObjects = this->backRightNode->GetGameObjectsToDraw();
			intersectingGameObjects.insert(std::end(intersectingGameObjects), std::begin(auxGameObjects), std::end(auxGameObjects));
			auxGameObjects.clear();
		}
	}
	return intersectingGameObjects;
}

void Quadtree::SetQuadrantCapacity(int quadrantCapacity)
{
	this->quadrantCapacity = quadrantCapacity;
}

void Quadtree::SetMinCubeSize(float minCubeSize)
{
	this->minQuadrantDiagonalSquared = 3 * minCubeSize * minCubeSize;
}
