#include "StdAfx.h"
#include "ComponentCameraSample.h"

#include "ComponentTransform.h"

#include "FileSystem/Json.h"

#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

#include "Application.h"

#include "debugdraw.h"



ComponentCameraSample::ComponentCameraSample(const bool active, GameObject* owner):
	Component(ComponentType::CAMERASAMPLE, active, owner, true)
{
	influenceRadius = 1.0f;
	positionOffset = float3::zero;
	position = GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalPosition();
	isSampleFixedEnabled = false;
	isSampleFocusEnabled = false;
	isCombatCameraEnabled = false;
	isSampleKpPositionEnabled = false;
	isSampleKpRotationEnabled = false;
}

ComponentCameraSample::ComponentCameraSample(const ComponentCameraSample& componentCameraSample):
	Component(ComponentType::CAMERASAMPLE, componentCameraSample.IsEnabled(), componentCameraSample.GetOwner(), true),
	influenceRadius(componentCameraSample.influenceRadius),
	positionOffset(componentCameraSample.positionOffset),
	position(componentCameraSample.position),
	isSampleFixedEnabled(componentCameraSample.isSampleFixedEnabled),
	isSampleFocusEnabled(componentCameraSample.isSampleFocusEnabled),
	isCombatCameraEnabled(componentCameraSample.isCombatCameraEnabled),
	isSampleKpPositionEnabled(componentCameraSample.isSampleKpPositionEnabled),
	isSampleKpRotationEnabled(componentCameraSample.isSampleKpRotationEnabled)
{
}

ComponentCameraSample::~ComponentCameraSample()
{
}


void ComponentCameraSample::Draw() const
{
#ifdef ENGINE
	bool canDrawSample = 
		(IsEnabled() && GetOwner() == App->GetModule<ModuleScene>()->GetSelectedGameObject()) 
		|| GetOwner()->GetParent() == App->GetModule<ModuleScene>()->GetSelectedGameObject();

	if (!canDrawSample)
	{
		return;
	}

	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();
	float3 position = transform->GetGlobalPosition();

	if (isCombatCameraEnabled)
	{
		dd::sphere(position, dd::colors::Orange, influenceRadius);
	}
	else
	{
		dd::sphere(position, dd::colors::Yellow, influenceRadius);
	}
#endif
}

void ComponentCameraSample::InternalSave(Json& meta)
{
	meta["influenceRadius"] = (float) influenceRadius;

	meta["positionOffsetX"] = (float) positionOffset.x;
	meta["positionOffsetY"] = (float) positionOffset.y;
	meta["positionOffsetZ"] = (float) positionOffset.z;

	meta["positionFixedX"] = (float) positionFixed.x;
	meta["positionFixedY"] = (float) positionFixed.y;
	meta["positionFixedZ"] = (float) positionFixed.z;

	meta["focusOffsetX"] = (float) focusOffset.x;
	meta["focusOffsetY"] = (float) focusOffset.y;

	meta["KpPosition"] = (float) KpPosition;
	meta["KpRotation"] = (float) KpRotation;

	meta["isSampleFixedEnabled"] = (bool) isSampleFixedEnabled;
	meta["isSampleFocusEnabled"] = (bool) isSampleFocusEnabled;
	meta["isCombatCameraEnabled"] = (bool) isCombatCameraEnabled;
	meta["isSampleKpPositionEnabled"] = (bool) isSampleKpPositionEnabled;
	meta["isSampleKpRotationEnabled"] = (bool) isSampleKpRotationEnabled;

	position = GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalPosition();
	meta["positionX"] = (float) position.x;
	meta["positionY"] = (float) position.y;
	meta["positionZ"] = (float) position.z;
}

void ComponentCameraSample::InternalLoad(const Json& meta)
{
	
	influenceRadius = meta["influenceRadius"];

	positionOffset.x = meta["positionOffsetX"];
	positionOffset.y = meta["positionOffsetY"];
	positionOffset.z = meta["positionOffsetZ"];

	positionFixed.x = meta["positionFixedX"];
	positionFixed.y = meta["positionFixedY"];
	positionFixed.z = meta["positionFixedZ"];

	focusOffset.x = meta["focusOffsetX"];
	focusOffset.y = meta["focusOffsetY"];

	KpPosition = meta["KpPosition"];
	KpRotation = meta["KpRotation"];

	isSampleFixedEnabled = meta["isSampleFixedEnabled"];
	isSampleFocusEnabled = meta["isSampleFocusEnabled"];
	isCombatCameraEnabled = meta["isCombatCameraEnabled"];
	isSampleKpPositionEnabled = meta["isSampleKpPositionEnabled"];
	isSampleKpRotationEnabled = meta["isSampleKpRotationEnabled"];

	position.x = meta["positionX"];
	position.y = meta["positionY"];
	position.z = meta["positionZ"];
}
