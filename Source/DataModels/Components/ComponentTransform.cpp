#pragma warning (disable: 26495)

#include "ComponentTransform.h"
#include "ComponentLight.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

ComponentTransform::ComponentTransform(const bool active, GameObject* owner)
	: Component(ComponentType::TRANSFORM, active, owner, false), 
	pos(float3::zero), rot(Quat::identity), sca(float3::one), rotXYZ(float3::zero),
	localMatrix(float4x4::identity), globalMatrix(float4x4::identity)
{
}

ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::Update()
{
	// Empty for now
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

	CalculateMatrices();
}

const float3& ComponentTransform::GetGlobalPosition()
{
	//UpdateTransformMatrices();

	return globalPos;
}

const float4x4& ComponentTransform::GetGlobalRotation()
{
	//UpdateTransformMatrices();

	return globalRot;
}

const float3& ComponentTransform::GetGlobalScale()
{
	//UpdateTransformMatrices();

	return globalSca;
}

void ComponentTransform::CalculateMatrices()
{
	localMatrix = float4x4::FromTRS(pos, rot, sca);

	const GameObject* parent = GetOwner()->GetParent();

	if (parent)
	{
		const ComponentTransform* parentTransform = static_cast<ComponentTransform*>(parent->GetComponent(ComponentType::TRANSFORM));

		// Set local matrix
		//localMatrix = parentTransform->GetGlobalMatrix().Inverted().Mul(globalMatrix);

		// Set global matrix
		globalMatrix = parentTransform->GetGlobalMatrix().Mul(localMatrix);

		globalPos = globalMatrix.TranslatePart();
		globalRot = globalMatrix.RotatePart();
		globalSca = globalMatrix.GetScale();
	}
}

void ComponentTransform::UpdateTransformMatrices()
{
	CalculateMatrices();

	if (this->GetOwner()->GetChildren().empty())
		return;

	for (GameObject* child : this->GetOwner()->GetChildren())
	{
		ComponentTransform* childTransform = static_cast<ComponentTransform*>
			(child->GetComponent(ComponentType::TRANSFORM));
		childTransform->UpdateTransformMatrices();
	}
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