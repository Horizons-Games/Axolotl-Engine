#include "ComponentTransform.h"
#include "ComponentLight.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "Scene/Scene.h"

#include "Modules/ModuleDebugDraw.h"
#include "Modules/ModuleScene.h"
#ifndef ENGINE
	#include "Modules/ModuleEditor.h"
	#include "Windows/WindowDebug.h"
#endif // ENGINE

#include "Math/float3x3.h"

ComponentTransform::ComponentTransform(const bool active, GameObject* owner) :
	Component(ComponentType::TRANSFORM, active, owner, false),
	pos(float3::zero),
	rot(float4x4::identity),
	sca(float3::one),
	globalPos(float3::zero),
	globalRot(float4x4::identity),
	globalSca(float3::one),
	rotXYZ(float3::zero),
	localMatrix(float4x4::identity),
	globalMatrix(float4x4::identity),
	localAABB({ { 0, 0, 0 }, { 0, 0, 0 } }),
	encapsuledAABB(localAABB),
	objectOBB({ localAABB }),
	drawBoundingBoxes(false)
{
}

ComponentTransform::ComponentTransform(const ComponentTransform& componentTransform) :
	Component(componentTransform),
	pos(componentTransform.GetPosition()),
	rot(componentTransform.GetRotation()),
	sca(componentTransform.GetScale()),
	globalPos(componentTransform.GetGlobalPosition()),
	globalRot(componentTransform.GetGlobalRotation()),
	globalSca(componentTransform.GetGlobalScale()),
	rotXYZ(componentTransform.GetRotationXYZ()),
	localMatrix(componentTransform.GetLocalMatrix()),
	globalMatrix(componentTransform.GetGlobalMatrix()),
	localAABB(componentTransform.localAABB),
	encapsuledAABB(localAABB),
	drawBoundingBoxes(false)
{
}

ComponentTransform::~ComponentTransform()
{
}

void ComponentTransform::Draw() const
{
#ifndef ENGINE
	if (App->GetModule<ModuleEditor>()->GetDebugOptions()->GetDrawBoundingBoxes())
	{
		App->GetModule<ModuleDebugDraw>()->DrawBoundingBox(objectOBB);
	}
#endif // ENGINE
	if (drawBoundingBoxes)
	{
		App->GetModule<ModuleDebugDraw>()->DrawBoundingBox(objectOBB);
	}
}

void ComponentTransform::SaveOptions(Json& meta)
{
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = static_cast<bool>(active);
	meta["removed"] = static_cast<bool>(canBeRemoved);

	meta["localPos_X"] = static_cast<float>(pos.x);
	meta["localPos_Y"] = static_cast<float>(pos.y);
	meta["localPos_Z"] = static_cast<float>(pos.z);

	meta["localRot_X"] = static_cast<float>(rotXYZ.x);
	meta["localRot_Y"] = static_cast<float>(rotXYZ.y);
	meta["localRot_Z"] = static_cast<float>(rotXYZ.z);

	meta["localSca_X"] = static_cast<float>(sca.x);
	meta["localSca_Y"] = static_cast<float>(sca.y);
	meta["localSca_Z"] = static_cast<float>(sca.z);
}

void ComponentTransform::LoadOptions(Json& meta)
{
	type = GetTypeByName(meta["type"]);
	active = static_cast<bool>(meta["active"]);
	canBeRemoved = static_cast<bool>(meta["removed"]);

	pos.x = static_cast<float>(meta["localPos_X"]);
	pos.y = static_cast<float>(meta["localPos_Y"]);
	pos.z = static_cast<float>(meta["localPos_Z"]);

	float3 rotation;
	rotation.x = static_cast<float>(meta["localRot_X"]);
	rotation.y = static_cast<float>(meta["localRot_Y"]);
	rotation.z = static_cast<float>(meta["localRot_Z"]);
	SetRotation(rotation);

	sca.x = static_cast<float>(meta["localSca_X"]);
	sca.y = static_cast<float>(meta["localSca_Y"]);
	sca.z = static_cast<float>(meta["localSca_Z"]);

	CalculateMatrices();
}

void ComponentTransform::CalculateMatrices()
{
	localMatrix = float4x4::FromTRS(pos, rot, sca);

	const GameObject* parent = GetOwner()->GetParent();

	if (parent)
	{
		ComponentTransform* parentTransform = (ComponentTransform*) (parent->GetComponent(ComponentType::TRANSFORM));

		// Set global matrix
		globalMatrix = parentTransform->GetGlobalMatrix().Mul(localMatrix);

		globalMatrix.Decompose(globalPos, globalRot, globalSca);
	}
	else
	{
		globalMatrix = localMatrix;
	}
}

const float4x4 ComponentTransform::CalculatePaletteGlobalMatrix()
{
	localMatrix = float4x4::FromTRS(pos, rot, sca);

	const GameObject* parent = GetOwner()->GetParent();
	const GameObject* root = GetOwner()->GetRootGO();

	if (parent != root)
	{
		ComponentTransform* parentTransform = (ComponentTransform*) (parent->GetComponent(ComponentType::TRANSFORM));

		return parentTransform->CalculatePaletteGlobalMatrix().Mul(localMatrix);
	}
	else
	{
		return localMatrix;
	}
}

void ComponentTransform::UpdateTransformMatrices()
{
	CalculateMatrices();
	for (Component* components : GetOwner()->GetComponents())
	{
		components->OnTransformChanged();
	}

	if (GetOwner()->GetChildren().empty())
		return;

	for (GameObject* child : GetOwner()->GetChildren())
	{
		ComponentTransform* childTransform =
			static_cast<ComponentTransform*>(child->GetComponent(ComponentType::TRANSFORM));
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
				App->GetModule<ModuleScene>()->GetLoadedScene()->RenderDirectionalLight();
			break;

		case LightType::POINT:
			if (translationModified)
			{
				App->GetModule<ModuleScene>()->GetLoadedScene()->UpdateScenePointLights();
				App->GetModule<ModuleScene>()->GetLoadedScene()->RenderPointLights();
			}
			break;

		case LightType::SPOT:
			if (translationModified || rotationModified)
			{
				App->GetModule<ModuleScene>()->GetLoadedScene()->UpdateSceneSpotLights();
				App->GetModule<ModuleScene>()->GetLoadedScene()->RenderSpotLights();
			}
			break;
	}
}

void ComponentTransform::CalculateBoundingBoxes()
{
	objectOBB = localAABB;
	objectOBB.Transform(globalMatrix);
	encapsuledAABB = objectOBB.MinimalEnclosingAABB();
}
