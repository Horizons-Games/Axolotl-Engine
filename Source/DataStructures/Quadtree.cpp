#pragma warning (disable: 4804)

#include "Quadtree.h"
#include "GameObject/GameObject.h"
#include "Components/ComponentTransform.h"
#include "Application.h"

#include "math/float4x4.h"
#include "math/float3.h"
#include "geometry/OBB.h"
#include "geometry/AABB.h"

#include "ModuleScene.h"
#include "Scene/Scene.h"

#include <queue>

Quadtree::Quadtree(const AABB& boundingBox) : boundingBox(boundingBox), parent(nullptr), isFreezed(false),
	quadrantCapacity(QUADRANT_CAPACITY), minQuadrantSideSize(MIN_CUBE_SIZE), 
	minQuadrantDiagonalSquared(3 * MIN_CUBE_SIZE * MIN_CUBE_SIZE), // D^2 = 3C^2
	frontRightNode(nullptr), frontLeftNode(nullptr), backLeftNode(nullptr), backRightNode(nullptr)
{
}

Quadtree::Quadtree(const AABB& boundingBox, Quadtree* parent) :
	boundingBox(boundingBox), parent(parent), isFreezed(false),
	quadrantCapacity(QUADRANT_CAPACITY), minQuadrantSideSize(MIN_CUBE_SIZE),
	minQuadrantDiagonalSquared(3 * MIN_CUBE_SIZE * MIN_CUBE_SIZE), // D^2 = 3C^2
	frontRightNode(nullptr), frontLeftNode(nullptr), backLeftNode(nullptr), backRightNode(nullptr)
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

	if (parent == nullptr && !EntireInQuadrant(gameObject) && !isFreezed )
	{
		ExpandToFit(gameObject);
	}
	else if (!InQuadrant(gameObject))
	{
		return;
	}
	else
	{
		if (IsLeaf())
		{
			if (gameObjects.size() < quadrantCapacity)
			{
				gameObjects.insert(gameObject);
			}
			else if (boundingBox.Diagonal().LengthSq() <= minQuadrantDiagonalSquared)
			{
				gameObjects.insert(gameObject);
			}
			else
			{
				Subdivide();
				RedistributeGameObjects(gameObject);
			}
		}
		else
		{

			bool inFrontRight = frontRightNode->InQuadrant(gameObject);
			bool inFrontLeft = frontLeftNode->InQuadrant(gameObject);
			bool inBackRight = backRightNode->InQuadrant(gameObject);
			bool inBackLeft = backLeftNode->InQuadrant(gameObject);

			if (inFrontRight && inFrontLeft && inBackRight && inBackLeft)
			{
				gameObjects.insert(gameObject);
			}
			else if (!inFrontRight && !inFrontLeft && !inBackRight && !inBackLeft)
			{
				gameObjects.insert(gameObject);
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
}

void Quadtree::GetFamilyObjects(std::set<GameObject*>& familyGameObjects)
{
	for (GameObject* gameObject : gameObjects)
	{
		familyGameObjects.insert(gameObject);
	}

	if (!IsLeaf())
	{
		std::set<GameObject*> familyGameObjectsChildFR;
		frontRightNode->GetFamilyObjects(familyGameObjectsChildFR);
		for (GameObject* gameObject : familyGameObjectsChildFR)
		{
			familyGameObjects.insert(gameObject);
		}

		std::set<GameObject*> familyGameObjectsChildFL;
		frontLeftNode->GetFamilyObjects(familyGameObjectsChildFL);
		for (GameObject* gameObject : familyGameObjectsChildFL)
		{
			familyGameObjects.insert(gameObject);
		}

		std::set<GameObject*> familyGameObjectsChildBR;
		backRightNode->GetFamilyObjects(familyGameObjectsChildBR);
		for (GameObject* gameObject : familyGameObjectsChildBR)
		{
			familyGameObjects.insert(gameObject);
		}

		std::set<GameObject*> familyGameObjectsChildBL;
		backLeftNode->GetFamilyObjects(familyGameObjectsChildBL);
		for (GameObject* gameObject : familyGameObjectsChildBL)
		{
			familyGameObjects.insert(gameObject);
		}
	}
}

bool Quadtree::Remove(const GameObject* gameObject)
{
	bool removed = false;
	if ((!IsLeaf() && !gameObjects.empty()))
	{
		std::set<GameObject*>::iterator it =
			std::find(gameObjects.begin(), gameObjects.end(), gameObject);
		if (it != gameObjects.end())
		{
			gameObjects.erase(it);
			removed = SmartRemove();
		}
		else 
		{
			bool childrenRemovedObject = frontRightNode->Remove(gameObject);
			childrenRemovedObject += frontLeftNode->Remove(gameObject);
			childrenRemovedObject += backRightNode->Remove(gameObject);
			childrenRemovedObject += backLeftNode->Remove(gameObject);

			if (childrenRemovedObject)
			{
				removed = SmartRemove();
			}
		}
	}
	else if (IsLeaf())
	{
		std::set<GameObject*>::iterator it =
			std::find(gameObjects.begin(), gameObjects.end(), gameObject);
		if (it != gameObjects.end())
		{
			gameObjects.erase(it);
			removed = true;
		}
	}
	else
	{
		bool childrenRemovedObject = frontRightNode->Remove(gameObject);
		childrenRemovedObject += frontLeftNode->Remove(gameObject);
		childrenRemovedObject += backRightNode->Remove(gameObject);
		childrenRemovedObject += backLeftNode->Remove(gameObject);

		if (childrenRemovedObject)
		{
			removed = SmartRemove();
		}
	}
	return removed;

}

bool Quadtree::SmartRemove()
{
	bool childrensDeleted = false;
	std::set<GameObject*> familyObjects = {};
	GetFamilyObjects(familyObjects);
	if (familyObjects.size() <= quadrantCapacity)
	{
		gameObjects.clear();
		gameObjects = familyObjects;
		ResetChildren();
		childrensDeleted = true;
	}
	return childrensDeleted;
}

bool Quadtree::InQuadrant(GameObject* gameObject)
{	
	const AABB& objectAABB = gameObject->GetEncapsuledAABB();
	return boundingBox.minPoint.x <= objectAABB.maxPoint.x&&
		boundingBox.minPoint.y <= objectAABB.maxPoint.y&&
		boundingBox.minPoint.z <= objectAABB.maxPoint.z&&
		objectAABB.minPoint.x <= boundingBox.maxPoint.x&&
		objectAABB.minPoint.y <= boundingBox.maxPoint.y&&
		objectAABB.minPoint.z <= boundingBox.maxPoint.z;
}

bool Quadtree::EntireInQuadrant(GameObject* gameObject)
{
	const AABB& objectAABB = gameObject->GetEncapsuledAABB();
	return boundingBox.minPoint.x <= objectAABB.minPoint.x &&
		boundingBox.minPoint.y <= objectAABB.minPoint.y &&
		boundingBox.minPoint.z <= objectAABB.minPoint.z &&
		objectAABB.maxPoint.x <= boundingBox.maxPoint.x &&
		objectAABB.maxPoint.y <= boundingBox.maxPoint.y &&
		objectAABB.maxPoint.z <= boundingBox.maxPoint.z;
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
		
		frontRightNode = std::make_unique<Quadtree>(quadrantBoundingBox, this);
	}
	
	if (frontLeftNode == nullptr)
	{
		quadrantBoundingBox.SetFromCenterAndSize(newCenterFrontLeft, newSize);
		frontLeftNode = std::make_unique<Quadtree>(quadrantBoundingBox, this);
	}
	
	if (backRightNode == nullptr)
	{
		quadrantBoundingBox.SetFromCenterAndSize(newCenterBackRight, newSize);
		backRightNode = std::make_unique<Quadtree>(quadrantBoundingBox, this);
	}
	
	if (backLeftNode == nullptr)
	{
		quadrantBoundingBox.SetFromCenterAndSize(newCenterBackLeft, newSize);
		backLeftNode = std::make_unique<Quadtree>(quadrantBoundingBox, this);
	}
}

void Quadtree::RedistributeGameObjects(GameObject* gameObject)
{
	// GameObject redistribution part
	gameObjects.insert(gameObject);

	for (std::set<GameObject*>::iterator it = std::begin(gameObjects); it != std::end(gameObjects);)
	{
		if (*it)
		{
			bool inFrontRight = frontRightNode->InQuadrant(*it);
			bool inFrontLeft = frontLeftNode->InQuadrant(*it);
			bool inBackRight = backRightNode->InQuadrant(*it);
			bool inBackLeft = backLeftNode->InQuadrant(*it);

			if (inFrontRight && inFrontLeft && inBackRight && inBackLeft)
			{
				++it;
			}
			else if (!inFrontRight && !inFrontLeft && !inBackRight && !inBackLeft)
			{
				++it;
			}
			else
			{
				if (inFrontRight) frontRightNode->Add(*it);
				if (inFrontLeft) frontLeftNode->Add(*it);
				if (inBackRight) backRightNode->Add(*it);
				if (inBackLeft) backLeftNode->Add(*it);
				it = gameObjects.erase(it);
			}
		}
	}
}

void Quadtree::ExpandToFit(GameObject* gameObject)
{
	float quadTreeMaxX = boundingBox.MaxX();
	float quadTreeMaxY = boundingBox.MaxY();
	float quadTreeMaxZ = boundingBox.MaxZ();
	float quadTreeMinX = boundingBox.MinX();
	float quadTreeMinY = boundingBox.MinY();
	float quadTreeMinZ = boundingBox.MinZ();
	float3 newMaxPoint = GetBoundingBox().maxPoint;
	float3 newMinPoint = GetBoundingBox().minPoint;
	
	const AABB& gameObjectAABB = gameObject->GetEncapsuledAABB();
	float3 gameObjectMaxPoint = gameObjectAABB.maxPoint;
	float3 gameObjectMinPoint = gameObjectAABB.minPoint;

	if (gameObjectMaxPoint.y > quadTreeMaxY || gameObjectMinPoint.y < quadTreeMinY)
	{
		if (gameObjectMinPoint.y < quadTreeMinY)
		{
			
			newMinPoint.y = -newMinPoint.y + 2* gameObjectMinPoint.y;
		}
		if (gameObjectMaxPoint.y > quadTreeMaxY)
		{
			
			newMaxPoint.y = -newMaxPoint.y + 2 * gameObjectMaxPoint.y;
		}
		AdjustHeightToNodes(newMinPoint.y, newMaxPoint.y);
	}

	if (!EntireInQuadrant(gameObject))
	{
		std::unique_ptr<Quadtree> newRootQuadTree = nullptr;
		if (gameObjectMaxPoint.x > quadTreeMaxX && gameObjectMaxPoint.z > quadTreeMaxZ)
		{
			newMaxPoint.x = quadTreeMinX + ((quadTreeMaxX - quadTreeMinX) * 2);
			newMaxPoint.z = quadTreeMinZ + ((quadTreeMaxZ - quadTreeMinZ) * 2);

			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = std::make_unique<Quadtree>(newAABB);
			newRootQuadTree->backRightNode = App->scene->GetLoadedScene()->GiveOwnershipOfQuadtree();
		}
		else if (gameObjectMinPoint.x < quadTreeMinX && gameObjectMinPoint.z < quadTreeMinZ)
		{
			newMinPoint.x = quadTreeMaxX - ((quadTreeMaxX - quadTreeMinX) * 2);
			newMinPoint.z = quadTreeMaxZ - ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = std::make_unique<Quadtree>(newAABB);
			newRootQuadTree->frontLeftNode = App->scene->GetLoadedScene()->GiveOwnershipOfQuadtree(); 
		}
		else if (gameObjectMaxPoint.z > quadTreeMaxZ && gameObjectMinPoint.x < quadTreeMinX)
		{
			newMinPoint.x = quadTreeMaxX - ((quadTreeMaxX - quadTreeMinX) * 2);
			newMaxPoint.z = quadTreeMinZ + ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = std::make_unique<Quadtree>(newAABB);
			newRootQuadTree->backRightNode = App->scene->GetLoadedScene()->GiveOwnershipOfQuadtree();
		}
		else if (gameObjectMinPoint.z < quadTreeMinZ && gameObjectMaxPoint.x > quadTreeMaxX)
		{
			newMaxPoint.x = quadTreeMinX + ((quadTreeMaxX - quadTreeMinX) * 2);
			newMinPoint.z = quadTreeMaxZ - ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = std::make_unique<Quadtree>(newAABB);
			newRootQuadTree->backLeftNode = App->scene->GetLoadedScene()->GiveOwnershipOfQuadtree();
		}
		else if (gameObjectMaxPoint.x > quadTreeMaxX || gameObjectMaxPoint.z > quadTreeMaxZ)
		{
			newMaxPoint.x = quadTreeMinX + ((quadTreeMaxX - quadTreeMinX) * 2);
			newMaxPoint.z = quadTreeMinZ + ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = std::make_unique<Quadtree>(newAABB);
			newRootQuadTree->backLeftNode = App->scene->GetLoadedScene()->GiveOwnershipOfQuadtree();
		}
		else if (gameObjectMinPoint.x < quadTreeMinX || gameObjectMinPoint.z < quadTreeMinZ)
		{
			newMinPoint.x = quadTreeMaxX - ((quadTreeMaxX - quadTreeMinX) * 2);
			newMinPoint.z = quadTreeMaxZ - ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = std::make_unique<Quadtree>(newAABB);
			newRootQuadTree->frontRightNode = App->scene->GetLoadedScene()->GiveOwnershipOfQuadtree();
		}
		newRootQuadTree->Subdivide();
		App->scene->GetLoadedScene()->SetSceneQuadTree(std::move(newRootQuadTree));
		parent = App->scene->GetLoadedScene()->GetSceneQuadTree();
		parent->Add(gameObject); 
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
		frontRightNode->AdjustHeightToNodes(minY, maxY);
		frontRightNode->AdjustHeightToNodes(minY, maxY);
		frontRightNode->AdjustHeightToNodes(minY, maxY);
		frontRightNode->AdjustHeightToNodes(minY, maxY);
	}

}

void Quadtree::ResetChildren()
{
	if (frontLeftNode != nullptr)
	{
		frontLeftNode = nullptr;
	}

	if (frontRightNode != nullptr)
	{
		frontRightNode = nullptr;
	}

	if (backLeftNode != nullptr)
	{
		backLeftNode = nullptr;
	}

	if (backRightNode != nullptr)
	{
		backRightNode = nullptr;
	}
}

void Quadtree::AddGameObjectAndChildren(GameObject* gameObject)
{
	if (gameObject->GetParent() == nullptr)
	{
		return;
	}
	std::list<GameObject*> familyObjects = {};
	std::list<GameObject*> objects = GetAllGameObjects(gameObject);
	familyObjects.insert(familyObjects.end(), objects.begin(), objects.end());
	for (GameObject* children : familyObjects)
	{
		App->scene->GetLoadedScene()->GetSceneQuadTree()->Add(children);
	}
}


void Quadtree::RemoveGameObjectAndChildren(const GameObject* gameObject)
{
	if (gameObject == nullptr || gameObject->GetParent() == nullptr)
	{
		return;
	}

	if (!gameObject->GetChildren().empty())
	{
		for (GameObject* children : gameObject->GetChildren())
		{
			RemoveGameObjectAndChildren(children);
		}
	}
	Remove(gameObject);
}

std::list<GameObject*> Quadtree::GetAllGameObjects(GameObject* gameObject)
{
	std::list<GameObject*> familyObjects = {};
	familyObjects.push_back(gameObject);
	for (GameObject* children : gameObject->GetChildren())
	{
		std::list<GameObject*> objectsChildren = GetAllGameObjects(children);
		familyObjects.insert(familyObjects.end(), objectsChildren.begin(), objectsChildren.end());
	}
	return familyObjects;
}

void Quadtree::CheckRaycastIntersection(std::map<float, const GameObject*>& hitGameObjects, const LineSegment& ray)
{
	std::queue<const Quadtree*> quadtreeQueue;
	quadtreeQueue.push(this);

	while (!quadtreeQueue.empty())
	{
		const Quadtree* currentQuadtree = quadtreeQueue.front();
		quadtreeQueue.pop();

		if (!ray.Intersects(currentQuadtree->boundingBox))
		{
			continue;
		}

		std::set<GameObject*> quadtreeGameObjects = currentQuadtree->gameObjects;

		float nearDistance, farDistance;
		for (GameObject* gameObject : quadtreeGameObjects)
		{
			bool hit = ray.Intersects(gameObject->GetEncapsuledAABB(), nearDistance, farDistance);

			if (hit && gameObject->IsActive())
			{
				hitGameObjects[nearDistance] = gameObject;
			}
		}

		if (currentQuadtree->IsLeaf()) 
		{
			continue;
		}

		quadtreeQueue.push(currentQuadtree->GetFrontRightNode());
		quadtreeQueue.push(currentQuadtree->GetFrontLeftNode());
		quadtreeQueue.push(currentQuadtree->GetBackRightNode());
		quadtreeQueue.push(currentQuadtree->GetBackLeftNode());
	}
}