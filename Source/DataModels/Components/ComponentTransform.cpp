#pragma warning (disable: 26495)

#include "ComponentTransform.h"
#include "ComponentLight.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "ModuleDebugDraw.h"

#include "GameObject/GameObject.h"
#include "Scene/Scene.h"
#include "FileSystem/Json.h"

#include "Math/float3x3.h"

ComponentTransform::ComponentTransform(const bool active, GameObject* owner)
	: Component(ComponentType::TRANSFORM, active, owner, false)
{
}

void ComponentTransform::Update()
{
	// Empty for now
}

void ComponentTransform::CalculateLightTransformed(const ComponentLight* lightComponent,
												   bool translationModified, 
												   bool rotationModified)
{
	switch (lightComponent->GetLightType())
	{
	case LightType::DIRECTIONAL:
		if (rotationModified)
			App->scene->GetLoadedScene()->RenderDirectionalLight();
		break;

	case LightType::POINT:
		if (translationModified)
		{
			App->scene->GetLoadedScene()->UpdateScenePointLights();
			App->scene->GetLoadedScene()->RenderPointLights();
		}
		break;

	case LightType::SPOT:
		if (translationModified || rotationModified)
		{
			App->scene->GetLoadedScene()->UpdateSceneSpotLights();
			App->scene->GetLoadedScene()->RenderSpotLights();
		}
		break;
	}
}

void ComponentTransform::SaveOptions(Json& meta)
{
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool) active;
	meta["removed"] = (bool) canBeRemoved;

	meta["localPos_X"] = (float)pos.x;
	meta["localPos_Y"] = (float)pos.y;
	meta["localPos_Z"] = (float)pos.z;

	float3 rotation = GetRotationXYZ();
	meta["localRot_X"] = (float)rotation.x;
	meta["localRot_Y"] = (float)rotation.y;
	meta["localRot_Z"] = (float)rotation.z;

	meta["localSca_X"] = (float)sca.x;
	meta["localSca_Y"] = (float)sca.y;
	meta["localSca_Z"] = (float)sca.z;
}

void ComponentTransform::LoadOptions(Json& meta)
{
	type = GetTypeByName(meta["type"]);
	active = (bool) meta["active"];
	//owner = (GameObject*) meta["owner"];
	canBeRemoved = (bool) meta["removed"];

	pos.x = (float) meta["localPos_X"];
	pos.y = (float) meta["localPos_Y"];
	pos.z = (float) meta["localPos_Z"];
		
	float3 rotation;
	rotation.x = (float) meta["localRot_X"];
	rotation.y = (float) meta["localRot_Y"];
	rotation.z = (float) meta["localRot_Z"];
	SetRotation(rotation);
				    
	sca.x = (float) meta["localSca_X"];
	sca.y = (float) meta["localSca_Y"];
	sca.z = (float) meta["localSca_Z"];

	CalculateLocalMatrix();
	if(GetOwner()->GetParent()) 
		CalculateGlobalMatrix();
}

void ComponentTransform::CalculateMatrices()
{
	//float4x4 localMatrix = float4x4::FromTRS((float3)GetPosition(), (float4x4)GetRotation(), (float3)GetScale());
	//SetLocalMatrix(localMatrix);

	const GameObject* parent = GetOwner()->GetParent();
	assert(parent);

	if (parent)
	{
		/*
		float3 parentPos, parentSca, localPos, localSca;
		float4x4 parentRot, localRot;

		ComponentTransform* parentTransform = static_cast<ComponentTransform*>(parent->GetComponent(ComponentType::TRANSFORM));

		parentTransform->GetGlobalMatrix().Decompose(parentPos, parentRot, parentSca);
		GetLocalMatrix().Decompose(localPos, localRot, localSca);

		float3 position = localPos + parentPos;
		float4x4 rotation = localRot * parentRot;
		float3 scale = parentSca.Mul(localSca);

		float4x4 globalMatrix = float4x4::FromTRS(position, rotation, scale);
		SetGlobalMatrix(globalMatrix);*/

		// Set local matrix
		localMatrix = std::static_pointer_cast<ComponentTransform>
			(parent->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix().Inverted().Mul(globalMatrix);

		// Set global matrix
		globalMatrix = std::static_pointer_cast<ComponentTransform>
			(parent->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix().Mul(localMatrix);

		globalPos = globalMatrix.TranslatePart();
		globalRot = globalMatrix.RotatePart().ToQuat().ToFloat4x4();
		globalSca = globalMatrix.GetScale();
	}
}

inline void ComponentTransform::UpdateTransformMatrices()
{
	CalculateMatrices();

	if (this->GetOwner().lock()->GetChildren().empty())
		return;

	for (std::weak_ptr<GameObject> child : this->GetOwner().lock()->GetChildren())
	{
		std::shared_ptr<GameObject> childAsShared = child.lock();
		std::shared_ptr<ComponentTransform> childTransform = 
			std::static_pointer_cast<ComponentTransform>(childAsShared->GetComponent(ComponentType::TRANSFORM));

		childTransform->UpdateTransformMatrices();
	}
}