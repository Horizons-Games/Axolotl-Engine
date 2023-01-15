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
#include "Scene.h"

Quadtree::Quadtree(const AABB& boundingBox) : boundingBox(boundingBox), parent(nullptr)
{
}

Quadtree::Quadtree(const AABB& boundingBox, Quadtree* parent) : boundingBox(boundingBox), parent(parent)
{
}

Quadtree::~Quadtree()
{
	gameObjects.clear();
	ResetChildren();
}

bool Quadtree::IsLeaf() const
{
	return frontLeftNode == nullptr;
}

void Quadtree::Add(GameObject* gameObject)
{
	assert(gameObject != nullptr);

	if (!InQuadrant(gameObject) && !isFreezed && parent == nullptr ) ExpandToFit(gameObject);
	else if (!InQuadrant(gameObject)) return;
	else 
	{
		if (IsLeaf())
		{
			if (gameObjects.size() < quadrantCapacity) gameObjects.push_back(gameObject);
			else if (boundingBox.Diagonal().LengthSq() <= minQuadrantDiagonalSquared) gameObjects.push_back(gameObject);
			else
			{
				Subdivide();
				RedistributeGameObjects(gameObject);
			}
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

std::list<GameObject*>& Quadtree::GetFamilyObjects()
{
	if (IsLeaf()) {
		return gameObjects;
	}
	else
	{
		std::list<GameObject*> familyGameObjects = gameObjects;

		familyGameObjects.splice(familyGameObjects.end(), frontRightNode->GetFamilyObjects());  //O(1) complexity
		familyGameObjects.splice(familyGameObjects.end(), frontLeftNode->GetFamilyObjects());
		familyGameObjects.splice(familyGameObjects.end(), backRightNode->GetFamilyObjects());
		familyGameObjects.splice(familyGameObjects.end(), backLeftNode->GetFamilyObjects());

		return familyGameObjects;
	}
}

void Quadtree::Remove(GameObject* gameObject)
{
	if ((!IsLeaf() && !gameObjects.empty())) 
	{
		std::list<GameObject*>::iterator it = std::find(gameObjects.begin(), gameObjects.end(), gameObject);
		if (it != gameObjects.end())
		{
			gameObjects.erase(it);
			SmartRemove();
		}
	}
	else if (IsLeaf()) 
	{
		std::list<GameObject*>::iterator it = std::find(gameObjects.begin(), gameObjects.end(), gameObject);
		if (it != gameObjects.end())
		{
			gameObjects.erase(it);
			if (parent != nullptr) parent->SmartRemove();
		}
	}
	else
	{
		frontRightNode->Remove(gameObject);
		if (!IsLeaf()) frontLeftNode->Remove(gameObject);
		if (!IsLeaf()) backRightNode->Remove(gameObject);
		if (!IsLeaf()) backLeftNode->Remove(gameObject);

		if (IsLeaf()) 
		{
			if(parent != nullptr) parent->OptimizeParentObjects();
		}
	}
}

void Quadtree::OptimizeParentObjects()
{
	if (GetFamilyObjects().size() <= quadrantCapacity)
	{
		gameObjects = GetFamilyObjects();
		ResetChildren();
		if (parent != nullptr) parent->OptimizeParentObjects();
	}
}

void Quadtree::SmartRemove()
{
	auto familyObjects = GetFamilyObjects();
	if (familyObjects.size() <= quadrantCapacity)
	{
		gameObjects = familyObjects;
		ResetChildren();
	}
}

bool Quadtree::InQuadrant(GameObject* gameObject)
{
	ComponentBoundingBoxes* boxes = (ComponentBoundingBoxes*)gameObject->GetComponent(ComponentType::BOUNDINGBOX);
	return boundingBox.Intersects(boxes->GetEncapsuledAABB());
}

void Quadtree::Subdivide()
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
	
	if (frontRightNode == nullptr) 
	{
		quadrantBoundingBox.SetFromCenterAndSize(newCenterFrontRight, newSize);
		frontRightNode = new Quadtree(quadrantBoundingBox, this);

	}
	if (frontLeftNode == nullptr)
	{
		quadrantBoundingBox.SetFromCenterAndSize(newCenterFrontLeft, newSize);
		frontLeftNode = new Quadtree(quadrantBoundingBox, this);
	}
	if (backRightNode == nullptr)
	{
		quadrantBoundingBox.SetFromCenterAndSize(newCenterBackRight, newSize);
		backRightNode = new Quadtree(quadrantBoundingBox, this);
	}
	if (backLeftNode == nullptr)
	{
		quadrantBoundingBox.SetFromCenterAndSize(newCenterBackLeft, newSize);
		backLeftNode = new Quadtree(quadrantBoundingBox, this);
	}
}

void Quadtree::RedistributeGameObjects(GameObject* gameObject) 
{
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
	
	if (!InQuadrant(gameObject))
	{
		Quadtree* newRootQuadTree = nullptr;
		if (gameObjectPosition.x > quadTreeMaxX && gameObjectPosition.z > quadTreeMaxZ)
		{
			newMaxPoint.x = quadTreeMinX + ((quadTreeMaxX - quadTreeMinX) * 2);
			newMaxPoint.z = quadTreeMinZ + ((quadTreeMaxZ - quadTreeMinZ) * 2);

			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = new Quadtree(newAABB, nullptr);
			newRootQuadTree->backRightNode = this;
		}
		else if (gameObjectPosition.x < quadTreeMinX && gameObjectPosition.z < quadTreeMinZ)
		{
			newMinPoint.x = quadTreeMaxX - ((quadTreeMaxX - quadTreeMinX) * 2);
			newMinPoint.z = quadTreeMaxZ - ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = new Quadtree(newAABB, nullptr);
			newRootQuadTree->frontLeftNode = this;
		}
		else if (gameObjectPosition.z > quadTreeMaxZ && gameObjectPosition.x < quadTreeMinX)
		{
			newMinPoint.x = quadTreeMaxX - ((quadTreeMaxX - quadTreeMinX) * 2);
			newMaxPoint.z = quadTreeMinZ + ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = new Quadtree(newAABB, nullptr);
			newRootQuadTree->frontRightNode = this;
		}
		else if (gameObjectPosition.z < quadTreeMinZ && gameObjectPosition.x > quadTreeMaxX)
		{
			newMaxPoint.x = quadTreeMinX + ((quadTreeMaxX - quadTreeMinX) * 2);
			newMinPoint.z = quadTreeMaxZ - ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = new Quadtree(newAABB, nullptr);
			newRootQuadTree->backLeftNode = this;
		}
		else if (gameObjectPosition.x > quadTreeMaxX || gameObjectPosition.z > quadTreeMaxZ)
		{
			newMaxPoint.x = quadTreeMinX + ((quadTreeMaxX - quadTreeMinX) * 2);
			newMaxPoint.z = quadTreeMinZ + ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = new Quadtree(newAABB, nullptr);
			newRootQuadTree->backRightNode = this;
		}
		else if (gameObjectPosition.x < quadTreeMinX || gameObjectPosition.z < quadTreeMinZ)
		{
			newMinPoint.x = quadTreeMaxX - ((quadTreeMaxX - quadTreeMinX) * 2);
			newMinPoint.z = quadTreeMaxZ - ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = new Quadtree(newAABB, nullptr);
			newRootQuadTree->frontLeftNode = this;
		}
		newRootQuadTree->Subdivide();
		App->scene->GetLoadedScene()->SetSceneQuadTree(newRootQuadTree);
		newRootQuadTree->Add(gameObject);
	}
	else 
	{
		Add(gameObject);
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

void Quadtree::ResetChildren()
{
	if (frontLeftNode != nullptr) 
	{
		delete frontLeftNode;
		frontLeftNode = nullptr;
	}
	
	if (frontRightNode != nullptr)
	{
		delete frontRightNode;
		frontRightNode = nullptr;
	}
	
	if (backLeftNode != nullptr)
	{
		delete backLeftNode;
		backLeftNode = nullptr;
	}

	if (backRightNode != nullptr)
	{
		delete backRightNode;
		backRightNode = nullptr;
	}
}
