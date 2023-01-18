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

Quadtree::Quadtree(const AABB& boundingBox, const std::shared_ptr<Quadtree>& parent) :
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

void Quadtree::Add(const std::shared_ptr<GameObject>& gameObject)
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

void Quadtree::GetFamilyObjects(std::list<std::weak_ptr<GameObject> >& familyGameObjects)
{
	//familyGameObjects.splice(familyGameObjects.end(), gameObjects);
	for (std::weak_ptr<GameObject> gameObject : gameObjects)
	{
		familyGameObjects.push_back(gameObject);
	}

	if (!IsLeaf())
	{
		std::list<std::weak_ptr<GameObject> > familyGameObjectsChildFR;
		frontRightNode->GetFamilyObjects(familyGameObjectsChildFR);
		for (std::weak_ptr<GameObject> gameObject : familyGameObjectsChildFR)
		{
			familyGameObjects.push_back(gameObject);
		}
		//familyGameObjects.splice(familyGameObjects.end(), familyGameObjects);  //O(1) complexity

		std::list<std::weak_ptr<GameObject> > familyGameObjectsChildFL;
		frontLeftNode->GetFamilyObjects(familyGameObjectsChildFL);
		for (std::weak_ptr<GameObject> gameObject : familyGameObjectsChildFL)
		{
			familyGameObjects.push_back(gameObject);
		}
		//frontLeftNode->GetFamilyObjects(familyGameObjects);
		//familyGameObjects.splice(familyGameObjects.end(), familyGameObjects);

		std::list<std::weak_ptr<GameObject> > familyGameObjectsChildBR;
		backRightNode->GetFamilyObjects(familyGameObjectsChildBR);
		for (std::weak_ptr<GameObject> gameObject : familyGameObjectsChildBR)
		{
			familyGameObjects.push_back(gameObject);
		}
		//backRightNode->GetFamilyObjects(familyGameObjects);
		//familyGameObjects.splice(familyGameObjects.end(), familyGameObjects);

		std::list<std::weak_ptr<GameObject> > familyGameObjectsChildBL;
		backLeftNode->GetFamilyObjects(familyGameObjectsChildBL);
		for (std::weak_ptr<GameObject> gameObject : familyGameObjectsChildBL)
		{
			familyGameObjects.push_back(gameObject);
		}
		//backLeftNode->GetFamilyObjects(familyGameObjects);
		//familyGameObjects.splice(familyGameObjects.end(), familyGameObjects);
	}
}

void Quadtree::Remove(const std::weak_ptr<GameObject>& gameObject)
{
	if ((!IsLeaf() && !gameObjects.empty()))
	{
		std::list<std::weak_ptr<GameObject>>::iterator it =
			std::find_if(gameObjects.begin(), gameObjects.end(),
				[gameObject](const std::weak_ptr<GameObject> obj) { return obj.lock() == gameObject.lock(); });
		if (it != gameObjects.end())
		{
			gameObjects.erase(it);
			SmartRemove();
		}
	}
	else if (IsLeaf())
	{
		std::list<std::weak_ptr<GameObject>>::iterator it =
			std::find_if(gameObjects.begin(), gameObjects.end(),
				[gameObject](const std::weak_ptr<GameObject> obj) { return obj.lock() == gameObject.lock(); });
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
	std::list<std::weak_ptr<GameObject> > familyObjects = {};
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
	std::list<std::weak_ptr<GameObject> > familyObjects = {};
	GetFamilyObjects(familyObjects);
	if (familyObjects.size() <= quadrantCapacity)
	{
		gameObjects.clear();
		gameObjects.splice(gameObjects.end(), familyObjects);
		ResetChildren();
	}
}

bool Quadtree::InQuadrant(const std::shared_ptr<GameObject>& gameObject)
{
	std::shared_ptr<ComponentBoundingBoxes> boxes =
		std::static_pointer_cast<ComponentBoundingBoxes>(gameObject->GetComponent(ComponentType::BOUNDINGBOX));
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
		frontRightNode = std::make_shared<Quadtree>(quadrantBoundingBox, shared_from_this());
	}
	if (frontLeftNode == nullptr)
	{
		quadrantBoundingBox.SetFromCenterAndSize(newCenterFrontLeft, newSize);
		frontLeftNode = std::make_shared<Quadtree>(quadrantBoundingBox, shared_from_this());
	}
	if (backRightNode == nullptr)
	{
		quadrantBoundingBox.SetFromCenterAndSize(newCenterBackRight, newSize);
		backRightNode = std::make_shared<Quadtree>(quadrantBoundingBox, shared_from_this());
	}
	if (backLeftNode == nullptr)
	{
		quadrantBoundingBox.SetFromCenterAndSize(newCenterBackLeft, newSize);
		backLeftNode = std::make_shared<Quadtree>(quadrantBoundingBox, shared_from_this());
	}
}

void Quadtree::RedistributeGameObjects(const std::shared_ptr<GameObject>& gameObject)
{
	// GameObject redistribution part
	gameObjects.push_back(gameObject);

	for (std::list<std::weak_ptr<GameObject> >::iterator it = gameObjects.begin(); it != gameObjects.end();)
	{
		std::shared_ptr<GameObject> go = (*it).lock();

		if (go)
		{
			bool inFrontRight = frontRightNode->InQuadrant(go);
			bool inFrontLeft = frontLeftNode->InQuadrant(go);
			bool inBackRight = backRightNode->InQuadrant(go);
			bool inBackLeft = backLeftNode->InQuadrant(go);

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
				it = gameObjects.erase(it);
				if (inFrontRight) frontRightNode->Add(go);
				if (inFrontLeft) frontLeftNode->Add(go);
				if (inBackRight) backRightNode->Add(go);
				if (inBackLeft) backLeftNode->Add(go);
			}
		}
	}
}

void Quadtree::ExpandToFit(const std::shared_ptr<GameObject>& gameObject)
{
	std::shared_ptr<ComponentTransform> gameObjectTransform =
		std::static_pointer_cast<ComponentTransform>(gameObject->GetComponent(ComponentType::TRANSFORM));
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
		std::shared_ptr<Quadtree> newRootQuadTree = nullptr;
		if (gameObjectPosition.x > quadTreeMaxX && gameObjectPosition.z > quadTreeMaxZ)
		{
			newMaxPoint.x = quadTreeMinX + ((quadTreeMaxX - quadTreeMinX) * 2);
			newMaxPoint.z = quadTreeMinZ + ((quadTreeMaxZ - quadTreeMinZ) * 2);

			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = std::make_shared<Quadtree>(newAABB, std::shared_ptr<Quadtree>());
			newRootQuadTree->backRightNode = shared_from_this();
		}
		else if (gameObjectPosition.x < quadTreeMinX && gameObjectPosition.z < quadTreeMinZ)
		{
			newMinPoint.x = quadTreeMaxX - ((quadTreeMaxX - quadTreeMinX) * 2);
			newMinPoint.z = quadTreeMaxZ - ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = std::make_shared<Quadtree>(newAABB, std::shared_ptr<Quadtree>());
			newRootQuadTree->frontLeftNode = shared_from_this();
		}
		else if (gameObjectPosition.z > quadTreeMaxZ && gameObjectPosition.x < quadTreeMinX)
		{
			newMinPoint.x = quadTreeMaxX - ((quadTreeMaxX - quadTreeMinX) * 2);
			newMaxPoint.z = quadTreeMinZ + ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = std::make_shared<Quadtree>(newAABB, std::shared_ptr<Quadtree>());
			newRootQuadTree->frontRightNode = shared_from_this();
		}
		else if (gameObjectPosition.z < quadTreeMinZ && gameObjectPosition.x > quadTreeMaxX)
		{
			newMaxPoint.x = quadTreeMinX + ((quadTreeMaxX - quadTreeMinX) * 2);
			newMinPoint.z = quadTreeMaxZ - ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = std::make_shared<Quadtree>(newAABB, std::shared_ptr<Quadtree>());
			newRootQuadTree->backLeftNode = shared_from_this();
		}
		else if (gameObjectPosition.x > quadTreeMaxX || gameObjectPosition.z > quadTreeMaxZ)
		{
			newMaxPoint.x = quadTreeMinX + ((quadTreeMaxX - quadTreeMinX) * 2);
			newMaxPoint.z = quadTreeMinZ + ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = std::make_shared<Quadtree>(newAABB, std::shared_ptr<Quadtree>());
			newRootQuadTree->backRightNode = shared_from_this();
		}
		else if (gameObjectPosition.x < quadTreeMinX || gameObjectPosition.z < quadTreeMinZ)
		{
			newMinPoint.x = quadTreeMaxX - ((quadTreeMaxX - quadTreeMinX) * 2);
			newMinPoint.z = quadTreeMaxZ - ((quadTreeMaxZ - quadTreeMinZ) * 2);
			AABB newAABB = AABB(newMinPoint, newMaxPoint);
			newRootQuadTree = std::make_shared<Quadtree>(newAABB, std::shared_ptr<Quadtree>());
			newRootQuadTree->frontLeftNode = shared_from_this();
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

void Quadtree::AddGameObjectAndChildren(const std::shared_ptr<GameObject>& gameObject)
{
	if (gameObject->GetParent().expired()) return;
	std::list<std::shared_ptr<GameObject> > familyObjects = {};
	std::list<std::weak_ptr<GameObject> > objects = GetAllGameObjects(gameObject);
	familyObjects.insert(familyObjects.end(), objects.begin(), objects.end());
	for (std::shared_ptr<GameObject> children : familyObjects)
	{
		App->scene->GetLoadedScene()->GetSceneQuadTree()->Add(children);
	}
}


void Quadtree::RemoveGameObjectAndChildren(const std::weak_ptr<GameObject>& gameObject)
{
	if (gameObject.lock()->GetParent().expired()) return;
	Remove(gameObject);

	std::shared_ptr<GameObject> asShared = gameObject.lock();

	if (!asShared->GetChildren().empty())
	{
		for (std::weak_ptr<GameObject> children : asShared->GetChildren())
		{
			RemoveGameObjectAndChildren(children);
		}
	}
}

std::list<std::weak_ptr<GameObject> > Quadtree::GetAllGameObjects(const std::weak_ptr<GameObject>& gameObject)
{
	std::list<std::weak_ptr<GameObject> > familyObjects = {};
	familyObjects.push_back(gameObject);
	for (std::weak_ptr<GameObject> children : gameObject.lock()->GetChildren())
	{
		std::list<std::weak_ptr<GameObject> > objectsChildren = GetAllGameObjects(children.lock());
		familyObjects.insert(familyObjects.end(), objectsChildren.begin(), objectsChildren.end());
	}
	return familyObjects;
}