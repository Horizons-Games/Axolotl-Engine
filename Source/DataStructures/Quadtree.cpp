#include "Quadtree.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentBoundingBoxes.h"
#include "Application.h"

#include "math/float4x4.h"
#include "math/float3.h"
#include "geometry/OBB.h"
#include "geometry/AABB.h"

#include "ModuleEngineCamera.h"
#include "ModuleScene.h"

Quadtree::Quadtree(const AABB& boundingBox) : boundingBox(boundingBox), parent(nullptr)
{
}

Quadtree::Quadtree(const AABB& boundingBox, Quadtree* parent) : boundingBox(boundingBox), parent(parent)
{
}

Quadtree::~Quadtree()
{
	ResetChildren();

	/*
	for (std::list<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
	{
		delete* it;
	}
	*/
	
	Clear();
	delete parent;

}

bool Quadtree::IsLeaf() const
{
	return frontLeftNode == nullptr;
}

void Quadtree::Add(GameObject* gameObject)
{
	assert(gameObject != nullptr);

	if (!InQuadrant(gameObject) && !isFreezed && parent != nullptr) ExpandQuadtree(gameObject);
	else if (!InQuadrant(gameObject)) return;
	else 
	{
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

	
}

Quadtree* Quadtree::Remove(GameObject* gameObject)
{
	if ((!IsLeaf() && !gameObjects.empty()) || IsLeaf()) 
	{
		std::list<GameObject*>::iterator it = std::find(gameObjects.begin(), gameObjects.end(), gameObject);
		if (it != gameObjects.end())
		{
			gameObjects.erase(it);
			return parent;
		}
			
	}
	
	else
	{
		frontRightNode->Remove(gameObject);
		frontLeftNode->Remove(gameObject);
		backRightNode->Remove(gameObject);
		backLeftNode->Remove(gameObject);
	}
}

void Quadtree::SmartRemove(GameObject* gameObject)
{
	Quadtree* quadrantWrapper = Remove(gameObject);
	if (quadrantWrapper != nullptr && quadrantWrapper->GetGameObjects().empty()) 
	{
		quadrantWrapper->Clear();
		quadrantWrapper->ResetChildren();
	}
}

bool Quadtree::InQuadrant(GameObject* gameObject)
{
	ComponentBoundingBoxes* boxes = (ComponentBoundingBoxes*)gameObject->GetComponent(ComponentType::BOUNDINGBOX);
	return boundingBox.Intersects(boxes->GetEncapsuledAABB());
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
	frontRightNode = new Quadtree(quadrantBoundingBox, this);
	
	quadrantBoundingBox.SetFromCenterAndSize(newCenterFrontLeft, newSize);
	frontLeftNode = new Quadtree(quadrantBoundingBox, this);

	quadrantBoundingBox.SetFromCenterAndSize(newCenterBackRight, newSize);
	backRightNode = new Quadtree(quadrantBoundingBox, this);

	quadrantBoundingBox.SetFromCenterAndSize(newCenterBackLeft, newSize);
	backLeftNode = new Quadtree(quadrantBoundingBox, this);

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

void Quadtree::ExpandQuadtree(GameObject* gameObject)
{

	ComponentBoundingBoxes* boxes = (ComponentBoundingBoxes*)gameObject->GetComponent(ComponentType::BOUNDINGBOX);
	boundingBox.Enclose(boxes->GetEncapsuledAABB());

	Clear();
	ResetChildren();
}

void Quadtree::ExpandToFit(GameObject* gameObject)
{
	ComponentTransform* gameObjectTransform = (ComponentTransform*)gameObject->GetComponent(ComponentType::TRANSFORM);
	float3 gameObjectPosition = gameObjectTransform->GetPosition();

	float quadTreeMaxX = this->boundingBox.MaxX();
	float quadTreeMaxY = this->boundingBox.MaxY();
	float quadTreeMaxZ = this->boundingBox.MaxZ();
	float quadTreeMinX = this->boundingBox.MinX();
	float quadTreeMinY = this->boundingBox.MinY();
	float quadTreeMinZ = this->boundingBox.MinZ();
	float3 newMaxPoint = GetBoundingBox().maxPoint;
	float3 newMinPoint = GetBoundingBox().minPoint;

	if (gameObjectPosition.y > quadTreeMaxY || gameObjectPosition.y < quadTreeMinY)
	{
		if (gameObjectPosition.y < quadTreeMinY) newMinPoint.y = gameObjectPosition.y;
		else newMaxPoint.y = gameObjectPosition.y;
		AdjustHeightToNodes(newMinPoint.y, newMaxPoint.y);
	}
	else if (!InQuadrant(gameObject))
	{
		if (gameObjectPosition.x > quadTreeMaxX && gameObjectPosition.z > quadTreeMaxZ)
		{
			newMaxPoint.x = quadTreeMinX + ((quadTreeMaxX - quadTreeMinX) * 2);
			newMaxPoint.z = quadTreeMinZ + ((quadTreeMaxZ - quadTreeMinZ) * 2);
		}
		else if (gameObjectPosition.x < quadTreeMinX && gameObjectPosition.z < quadTreeMinZ)
		{
			newMinPoint.x = quadTreeMaxX - ((quadTreeMaxX - quadTreeMinX) * 2);
			newMinPoint.z = quadTreeMaxZ - ((quadTreeMaxZ - quadTreeMinZ) * 2);
		}
		else if (gameObjectPosition.z > quadTreeMaxZ && gameObjectPosition.x < quadTreeMinX)
		{
			newMinPoint.x = quadTreeMaxX - ((quadTreeMaxX - quadTreeMinX) * 2);
			newMaxPoint.z = quadTreeMinZ + ((quadTreeMaxZ - quadTreeMinZ) * 2);
		}
		else if (gameObjectPosition.z < quadTreeMinZ && gameObjectPosition.x > quadTreeMaxX)
		{
			newMaxPoint.x = quadTreeMinX + ((quadTreeMaxX - quadTreeMinX) * 2);
			newMinPoint.z = quadTreeMaxZ - ((quadTreeMaxZ - quadTreeMinZ) * 2);
		}
		else if (gameObjectPosition.x > quadTreeMaxX || gameObjectPosition.z > quadTreeMaxZ)
		{
			newMaxPoint.x = quadTreeMinX + ((quadTreeMaxX - quadTreeMinX) * 2);
			newMaxPoint.z = quadTreeMinZ + ((quadTreeMaxZ - quadTreeMinZ) * 2);
		}
		else if (gameObjectPosition.x < quadTreeMinX || gameObjectPosition.z < quadTreeMinZ)
		{
			newMinPoint.x = quadTreeMaxX - ((quadTreeMaxX - quadTreeMinX) * 2);
			newMinPoint.z = quadTreeMaxZ - ((quadTreeMaxZ - quadTreeMinZ) * 2);
		}

		AABB newAABB = AABB(newMinPoint, newMaxPoint);
		Quadtree* newRootQuadTree = new Quadtree(newAABB, nullptr);
		App->scene->SetSceneQuadTree(newRootQuadTree);
		if (!newRootQuadTree->InQuadrant(gameObject)) newRootQuadTree->ExpandToFit(gameObject);
	}
}

void Quadtree::AdjustHeightToNodes(float minY, float maxY) 
{
	float3 newMaxPoint = GetBoundingBox().maxPoint;
	float3 newMinPoint = GetBoundingBox().minPoint;
	newMinPoint.y = minY;
	newMaxPoint.y = maxY;
	AABB newAABB = AABB(newMinPoint, newMaxPoint);
	SetBoundingBox(newAABB);
	if (!IsLeaf()) 
	{
		this->frontRightNode->AdjustHeightToNodes(minY, maxY);
		this->frontRightNode->AdjustHeightToNodes(minY, maxY);
		this->frontRightNode->AdjustHeightToNodes(minY, maxY);
		this->frontRightNode->AdjustHeightToNodes(minY, maxY);
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

void Quadtree::ResetChildren()
{
	delete frontLeftNode;
	delete frontRightNode;
	delete backLeftNode;
	delete backRightNode;
}

// Draw recursively in the scene
void Quadtree::Draw()
{
	if (App->engineCamera->IsInside(boundingBox) || App->scene->IsInsideACamera(boundingBox))
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
	if (App->engineCamera->IsInside(GetBoundingBox()) || App->scene->IsInsideACamera(GetBoundingBox()))
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
