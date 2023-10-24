#include "StdAfx.h"

#include "ComponentObstacle.h"

#include "Application.h"
#include "ModuleNavigation.h"
#include "ComponentTransform.h"

#include "GameObject/GameObject.h"
#include "FileSystem/Json.h"

ComponentObstacle::ComponentObstacle(bool active, GameObject* owner) :
	Component(ComponentType::OBSTACLE, active, owner, true)
{
	transform = GetOwner()->GetComponent<ComponentTransform>();

	//This should be done in the init
	currentPosition = transform->GetGlobalPosition();
	if (IsEnabled())
	{
		AddObstacle();
	}
}

ComponentObstacle::~ComponentObstacle()
{
	RemoveObstacle();
}

void ComponentObstacle::Update()
{
	if (obstacleReference != nullptr && !shouldAddObstacle)
	{
		return;
	}

	float3 newPosition = transform->GetGlobalPosition();
	float3 newRotation = transform->GetGlobalRotation().ToEulerXYZ();
	if (!newPosition.Equals(currentPosition) || !newRotation.Equals(currentRotation))
	{
		currentPosition = newPosition;
		currentRotation = newRotation;
		if (IsEnabled())
		{
			AddObstacle();
		}
	}

	if (shouldAddObstacle)
	{
		AddObstacle();
	}
}

void ComponentObstacle::AddObstacle()
{
	shouldAddObstacle = true;

	std::shared_ptr<ResourceNavMesh> navMesh = App->GetModule<ModuleNavigation>()->GetNavMesh();
	if (navMesh == nullptr || !navMesh->IsGenerated())
	{
		return;
	}

	dtTileCache* tileCache = navMesh->GetTileCache();
	if (!tileCache)
	{
		return;
	}

	RemoveObstacle();

	obstacleReference = new dtObstacleRef;

	// For some wierd reason, if I use the global transform variable the obstacle doesnt work correctly
	ComponentTransform* transform = GetOwner()->GetComponent<ComponentTransform>();
	float3 position = transform->GetGlobalPosition();
	position.y += yOffset;

	switch (obstacleType)
	{
		case ObstacleType::DT_OBSTACLE_CYLINDER:
			tileCache->addObstacle(&position[0], radius, height, obstacleReference, mustBeDrawnGizmo);
			break;
		default: // DT_OBSTACLE_BOX ||  DT_OBSTACLE_ORIENTED_BOX
			tileCache->addBoxObstacle(&position[0],
									  &(boxSize / 2)[0],
									  transform->GetGlobalRotation().ToEulerXYZ().y,
									  obstacleReference,
									  mustBeDrawnGizmo);
			break;
	}

	shouldAddObstacle = false;
}

void ComponentObstacle::RemoveObstacle()
{
	shouldAddObstacle = false;

	/*if (App->scene->scene != GetOwner().scene)
		return;*/
	std::shared_ptr<ResourceNavMesh> navMesh = App->GetModule<ModuleNavigation>()->GetNavMesh();
	if (navMesh == nullptr || !navMesh->IsGenerated())
	{
		return;
	}

	dtTileCache* tileCache = navMesh->GetTileCache();
	if (!tileCache || !obstacleReference)
	{
		return;
	}

	tileCache->removeObstacle(*obstacleReference);
	obstacleReference = nullptr;
}

void ComponentObstacle::SetRadius(float newRadius)
{
	if (obstacleType == ObstacleType::DT_OBSTACLE_CYLINDER)
	{
		radius = newRadius;
		AddObstacle();
	}
}

void ComponentObstacle::SetHeight(float newHeight)
{
	if (obstacleType == ObstacleType::DT_OBSTACLE_CYLINDER)
	{
		height = newHeight;
		AddObstacle();
	}
}

void ComponentObstacle::SetBoxSize(float3 size)
{
	if (obstacleType == ObstacleType::DT_OBSTACLE_BOX || obstacleType == ObstacleType::DT_OBSTACLE_ORIENTED_BOX)
	{
		boxSize = size;
		AddObstacle();
	}
}

void ComponentObstacle::SetObstacleType(ObstacleType newType)
{
	obstacleType = newType;
	AddObstacle();
}

void ComponentObstacle::ResetSize()
{
	if (obstacleType == ObstacleType::DT_OBSTACLE_CYLINDER)
	{
		radius = 1.0f;
		height = 2.0f;
	}
	else
	{
		boxSize = float3::one;
	}

	AddObstacle();
}

void ComponentObstacle::SetDrawGizmo(bool value)
{
	mustBeDrawnGizmo = value;
	AddObstacle();
}

void ComponentObstacle::SetYOffset(float value)
{
	yOffset = value;
	AddObstacle();
}

void ComponentObstacle::InternalSave(Json& meta)
{
	float3 size = GetBoxSize();
	meta["boxSize_X"] = static_cast<float>(size.x);
	meta["boxSize_Y"] = static_cast<float>(size.y);
	meta["boxSize_Z"] = static_cast<float>(size.z);
	meta["radius"] = static_cast<float>(GetRadius());
	meta["height"] = static_cast<float>(GetHeight());
	meta["obstacleType"] = static_cast<int>(obstacleType);
	meta["mustBeDrawnGizmo"] = static_cast<bool>(mustBeDrawnGizmo);
	meta["yOffset"] = static_cast<float>(yOffset);
}

void ComponentObstacle::InternalLoad(const Json& meta)
{
	SetBoxSize(float3(static_cast<float>(meta["boxSize_X"]),
					 static_cast<float>(meta["boxSize_Y"]),
					 static_cast<float>(meta["boxSize_Z"])));
	SetRadius(static_cast<float>(meta["radius"]));
	SetHeight(static_cast<float>(meta["height"]));
	SetObstacleType(static_cast<ObstacleType>(obstacleType));
	SetDrawGizmo(static_cast<bool>(meta["mustBeDrawnGizmo"]));
	SetYOffset(static_cast<float>(meta["yOffset"]));

	RemoveObstacle();
}