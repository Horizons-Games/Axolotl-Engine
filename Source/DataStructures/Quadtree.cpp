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
#include "Scene/Scene.h"
#include <list>

Quadtree::Quadtree(const AABB& boundingBox) : boundingBox(boundingBox), parent(nullptr)
{
}

Quadtree::Quadtree(const AABB& boundingBox, Quadtree* parent) :
	boundingBox(boundingBox), parent(parent)
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

void Quadtree::Add(const GameObject* gameObject)
{
	assert(gameObject != nullptr);

	if (!InQuadrant(gameObject) && !isFreezed && parent == nullptr) ExpandToFit(gameObject);
	else if (!InQuadrant(gameObject)) return;
	else
	{
		if (IsLeaf())
		{
			if (gameObjects.size() < quadrantCapacity) gameObjects.push_back(gameObject);
			else if (boundingBox.Diagonal().LengthSq() <= minQuadrantDiagonalSquared)
			{
				gameObjects.push_back(gameObject);
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

			if (inFrontRight && inFrontLeft && inBackRight && inBackLeft) gameObjects.push_back(gameObject);
			else if (!inFrontRight && !inFrontLeft && !inBackRight && !inBackLeft) gameObjects.push_back(gameObject);
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

void Quadtree::GetFamilyObjects(std::list<const GameObject*>& familyGameObjects)
{
	//familyGameObjects.splice(familyGameObjects.end(), gameObjects);
	for (const GameObject* gameObject : gameObjects)
	{
		familyGameObjects.push_back(gameObject);
	}

	if (!IsLeaf())
	{
		std::list<const GameObject*> familyGameObjectsChildFR;
		frontRightNode->GetFamilyObjects(familyGameObjectsChildFR);
		for (const GameObject* gameObject : familyGameObjectsChildFR)
		{
			familyGameObjects.push_back(gameObject);
		}
		//familyGameObjects.splice(familyGameObjects.end(), familyGameObjects);  //O(1) complexity

		std::list<const GameObject*> familyGameObjectsChildFL;
		frontLeftNode->GetFamilyObjects(familyGameObjectsChildFL);
		for (const GameObject* gameObject : familyGameObjectsChildFL)
		{
			familyGameObjects.push_back(gameObject);
		}
		//frontLeftNode->GetFamilyObjects(familyGameObjects);
		//familyGameObjects.splice(familyGameObjects.end(), familyGameObjects);

		std::list<const GameObject*> familyGameObjectsChildBR;
		backRightNode->GetFamilyObjects(familyGameObjectsChildBR);
		for (const GameObject* gameObject : familyGameObjectsChildBR)
		{
			familyGameObjects.push_back(gameObject);
		}
		//backRightNode->GetFamilyObjects(familyGameObjects);
		//familyGameObjects.splice(familyGameObjects.end(), familyGameObjects);

		std::list<const GameObject*> familyGameObjectsChildBL;
		backLeftNode->GetFamilyObjects(familyGameObjectsChildBL);
		for (const GameObject* gameObject : familyGameObjectsChildBL)
		{
			familyGameObjects.push_back(gameObject);
		}
		//backLeftNode->GetFamilyObjects(familyGameObjects);
		//familyGameObjects.splice(familyGameObjects.end(), familyGameObjects);
	}
}

void Quadtree::Remove(const GameObject* gameObject)
{
	if ((!IsLeaf() && !gameObjects.empty()))
	{
		std::list<const GameObject*>::iterator it =
			std::find(gameObjects.begin(), gameObjects.end(), gameObject);
		if (it != gameObjects.end())
		{
			gameObjects.erase(it);
			SmartRemove();
		}
		else {
			frontRightNode->Remove(gameObject);
			if (!IsLeaf()) frontLeftNode->Remove(gameObject);
			if (!IsLeaf()) backRightNode->Remove(gameObject);
			if (!IsLeaf()) backLeftNode->Remove(gameObject);

			if (IsLeaf())
			{
				if (parent != nullptr) parent->OptimizeParentObjects();
			}

		}
	}
	else if (IsLeaf())
	{
		std::list<const GameObject*>::iterator it =
			std::find(gameObjects.begin(), gameObjects.end(), gameObject);
		if (it != gameObjects.end())
		{
			gameObjects.erase(it);
			if (parent != nullptr)
			{
				parent->SmartRemove();
			}
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
			if (parent != nullptr)
			{
				parent->OptimizeParentObjects();
			}
		}
	}

}

void Quadtree::OptimizeParentObjects()
{
	std::list<const GameObject*> familyObjects = {};
	GetFamilyObjects(familyObjects);
	if (familyObjects.size() <= quadrantCapacity)
	{
		gameObjects.clear();
		gameObjects.splice(gameObjects.end(), familyObjects);
		ResetChildren();
		if (parent != nullptr)
		{
			parent->OptimizeParentObjects();
		}
	}
}

void Quadtree::SmartRemove()
{
	std::list<const GameObject*> familyObjects = {};
	GetFamilyObjects(familyObjects);
	if (familyObjects.size() <= quadrantCapacity)
	{
		gameObjects.clear();
		gameObjects.splice(gameObjects.end(), familyObjects);
		ResetChildren();
	}
}

bool Quadtree::InQuadrant(const GameObject* gameObject)
{
	ComponentBoundingBoxes* boxes =
		static_cast<ComponentBoundingBoxes*>(gameObject->GetComponent(ComponentType::BOUNDINGBOX));
	
	AABB objectAABB = boxes->GetEncapsuledAABB();
	return boundingBox.minPoint.x <= objectAABB.maxPoint.x&&
		boundingBox.minPoint.y <= objectAABB.maxPoint.y&&
		boundingBox.minPoint.z <= objectAABB.maxPoint.z&&
		objectAABB.minPoint.x <= boundingBox.maxPoint.x&&
		objectAABB.minPoint.y <= boundingBox.maxPoint.y&&
		objectAABB.minPoint.z <= boundingBox.maxPoint.z;
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

void Quadtree::RedistributeGameObjects(const GameObject* gameObject)
{
	// GameObject redistribution part
	gameObjects.push_back(gameObject);

	for (std::list<const GameObject*>::iterator it = std::begin(gameObjects); it != std::end(gameObjects);)
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

void Quadtree::ExpandToFit(const GameObject* gameObject)
{
	ComponentTransform* gameObjectTransform =
		static_cast<ComponentTransform*>(gameObject->GetComponent(ComponentType::TRANSFORM));
	ComponentBoundingBoxes* boxes =
		static_cast<ComponentBoundingBoxes*>(gameObject->GetComponent(ComponentType::BOUNDINGBOX));
	float3 gameObjectPosition = boxes->GetEncapsuledAABB().CenterPoint();

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
		if (gameObjectPosition.y < quadTreeMinY) newMinPoint.y = gameObjectPosition.y - boxes->GetEncapsuledAABB().Size().y;
		else newMaxPoint.y = gameObjectPosition.y + boxes->GetEncapsuledAABB().Size().y;
		AdjustHeightToNodes(newMinPoint.y, newMaxPoint.y);
	}

	if (!InQuadrant(gameObject))
	{
		std::unique_ptr<Quadtree> newRootQuadTree = nullptr;
		if (gameObjectPosition.x > quadTreeMaxX && gameObjectPosition.z > quadTreeMaxZ)
		{
			newMaxPoint.x = quadTreeMinX + ((quadTreeMaxX - quadTreeMinX) * 2);
			newMaxPoint.z = quadTreeMinZ + ((quadTreeMaxZ - quadTreeMinZ) * 2);

			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			//std::unique_ptr<A> uniquePtr = std::unique_ptr<A>(rawPointer);
			newRootQuadTree = std::make_unique<Quadtree>(newAABB);
			newRootQuadTree->backRightNode = App->scene->GetLoadedScene()->GiveOwnershipOfQuadtree();
		}
		else if (gameObjectPosition.x < quadTreeMinX && gameObjectPosition.z < quadTreeMinZ)
		{
			newMinPoint.x = quadTreeMaxX - ((quadTreeMaxX - quadTreeMinX) * 2);
			newMinPoint.z = quadTreeMaxZ - ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = std::make_unique<Quadtree>(newAABB);
			newRootQuadTree->frontLeftNode = App->scene->GetLoadedScene()->GiveOwnershipOfQuadtree(); //std::unique_ptr<Quadtree>(this);
		}
		else if (gameObjectPosition.z > quadTreeMaxZ && gameObjectPosition.x < quadTreeMinX)
		{
			newMinPoint.x = quadTreeMaxX - ((quadTreeMaxX - quadTreeMinX) * 2);
			newMaxPoint.z = quadTreeMinZ + ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = std::make_unique<Quadtree>(newAABB);
			newRootQuadTree->backRightNode = App->scene->GetLoadedScene()->GiveOwnershipOfQuadtree();
		}
		else if (gameObjectPosition.z < quadTreeMinZ && gameObjectPosition.x > quadTreeMaxX)
		{
			newMaxPoint.x = quadTreeMinX + ((quadTreeMaxX - quadTreeMinX) * 2);
			newMinPoint.z = quadTreeMaxZ - ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = std::make_unique<Quadtree>(newAABB);
			newRootQuadTree->backLeftNode = App->scene->GetLoadedScene()->GiveOwnershipOfQuadtree();
		}
		else if (gameObjectPosition.x > quadTreeMaxX || gameObjectPosition.z > quadTreeMaxZ)
		{
			newMaxPoint.x = quadTreeMinX + ((quadTreeMaxX - quadTreeMinX) * 2);
			newMaxPoint.z = quadTreeMinZ + ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = std::make_unique<Quadtree>(newAABB);
			newRootQuadTree->backLeftNode = App->scene->GetLoadedScene()->GiveOwnershipOfQuadtree();
		}
		else if (gameObjectPosition.x < quadTreeMinX || gameObjectPosition.z < quadTreeMinZ)
		{
			newMinPoint.x = quadTreeMaxX - ((quadTreeMaxX - quadTreeMinX) * 2);
			newMinPoint.z = quadTreeMaxZ - ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = std::make_unique<Quadtree>(newAABB);
			newRootQuadTree->frontRightNode = App->scene->GetLoadedScene()->GiveOwnershipOfQuadtree();
		}
		newRootQuadTree->Subdivide();
		App->scene->GetLoadedScene()->SetSceneQuadTree(std::move(newRootQuadTree));
		this->parent = App->scene->GetLoadedScene()->GetSceneQuadTree();
		this->parent->Add(gameObject); 
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

void Quadtree::AddGameObjectAndChildren(const GameObject* gameObject)
{
	if (gameObject->GetParent() == nullptr) return;
	std::list<const GameObject*> familyObjects = {};
	std::list<const GameObject*> objects = GetAllGameObjects(gameObject);
	familyObjects.insert(familyObjects.end(), objects.begin(), objects.end());
	for (const GameObject* children : familyObjects)
	{
		App->scene->GetLoadedScene()->GetSceneQuadTree()->Add(children);
	}
}


void Quadtree::RemoveGameObjectAndChildren(const GameObject* gameObject)
{
	if (gameObject == nullptr || gameObject->GetParent() == nullptr) return;
	Remove(gameObject);

	if (!gameObject->GetChildren().empty())
	{
		for (GameObject* children : gameObject->GetChildren())
		{
			RemoveGameObjectAndChildren(children);
		}
	}
}

std::list<const GameObject*> Quadtree::GetAllGameObjects(const GameObject* gameObject)
{
	std::list<const GameObject*> familyObjects = {};
	familyObjects.push_back(gameObject);
	for (GameObject* children : gameObject->GetChildren())
	{
		std::list<const GameObject*> objectsChildren = GetAllGameObjects(children);
		familyObjects.insert(familyObjects.end(), objectsChildren.begin(), objectsChildren.end());
	}
	return familyObjects;
}

// Speeding raycast function, this should be changed to an iterative function instead of a recursive function
void Quadtree::CheckRaycastIntersection(std::map<float, const GameObject*> hitGameObjects, const LineSegment& ray)
{
	if (!ray.Intersects(boundingBox))
	{
		return;
	}

	float nearDistance, farDistance;
	for (const GameObject* gameObject : gameObjects)
	{
		ComponentBoundingBoxes* componentBoundingBox = static_cast<ComponentBoundingBoxes*>
			(gameObject->GetComponent(ComponentType::BOUNDINGBOX));

		bool hit = ray.Intersects(componentBoundingBox->GetObjectOBB(), nearDistance, farDistance);

		if (hit && gameObject->IsActive())
		{
			hitGameObjects[nearDistance] = gameObject;
		}
	}

	if (frontRightNode != nullptr) frontRightNode->CheckRaycastIntersection(hitGameObjects, ray);
	if (frontLeftNode != nullptr) frontLeftNode->CheckRaycastIntersection(hitGameObjects, ray);
	if (backRightNode != nullptr) backRightNode->CheckRaycastIntersection(hitGameObjects, ray);
	if (backLeftNode != nullptr) backLeftNode->CheckRaycastIntersection(hitGameObjects, ray);
}