#pragma warning (disable: 26495)

#include "ComponentTransform.h"
#include "ComponentLight.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

#include "Math/float3x3.h"

ComponentTransform::ComponentTransform(const bool active, GameObject* owner)
	: Component(ComponentType::TRANSFORM, active, owner, false), 
	pos(float3::zero), rot(float4x4::identity), sca(float3::one), 
	globalPos(float3::zero), globalRot(float4x4::identity), globalSca(float3::one), 
	rotXYZ(float3::zero), localMatrix(float4x4::identity), globalMatrix(float4x4::identity)
{
}

ComponentTransform::ComponentTransform(const ComponentTransform& componentTransform)
	: Component(componentTransform),
	pos(componentTransform.GetPosition()), rot(componentTransform.GetRotation()),
	sca(componentTransform.GetScale()),	globalPos(componentTransform.GetGlobalPosition()),
	globalRot(componentTransform.GetGlobalRotation()), globalSca(componentTransform.GetGlobalScale()),
	rotXYZ(componentTransform.GetRotationXYZ()), localMatrix(componentTransform.GetLocalMatrix()),
	globalMatrix(componentTransform.GetGlobalMatrix())
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

void ComponentTransform::CalculateMatrices()
{
	localMatrix = float4x4::FromTRS(pos, rot, sca);

	const GameObject* parent = GetOwner()->GetParent();

	if (parent)
	{
		ComponentTransform* parentTransform = static_cast<ComponentTransform*>(parent->GetComponent(ComponentType::TRANSFORM));
		parentTransform->CalculateMatrices();

		// Set local matrix
		//localMatrix = parentTransform->GetGlobalMatrix().Inverted().Mul(globalMatrix);

		// Set global matrix
		globalMatrix = parentTransform->GetGlobalMatrix().Mul(localMatrix);

		globalPos = globalMatrix.TranslatePart();
		globalRot = static_cast<float4x4>(globalMatrix.RotatePart());
		globalSca = globalMatrix.GetScale();
	}
}

void ComponentTransform::UpdateTransformMatrices()
{
	CalculateMatrices();
	GetOwner()->Update();

	if (GetOwner()->GetChildren().empty())
		return;


	for (GameObject* child : GetOwner()->GetChildren())
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