#include "StdAfx.h"

#include "ComponentLight.h"
#include "ComponentTransform.h"

#include "Application.h"
#include "Scene/Scene.h"

#include "Modules/ModuleDebugDraw.h"
#include "Modules/ModuleScene.h"

ComponentTransform::ComponentTransform(const bool active, GameObject* owner) :
	Component(ComponentType::TRANSFORM, active, owner, false),
	localPos(float3::zero),
	localRot(Quat::identity),
	localSca(float3::one),
	globalPos(float3::zero),
	globalRot(Quat::identity),
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
	localPos(componentTransform.GetLocalPosition()),
	localRot(componentTransform.GetLocalRotation()),
	localSca(componentTransform.GetLocalScale()),
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

void ComponentTransform::InternalSave(Json& meta)
{
	meta["localPos_X"] = static_cast<float>(localPos.x);
	meta["localPos_Y"] = static_cast<float>(localPos.y);
	meta["localPos_Z"] = static_cast<float>(localPos.z);

	meta["localRot_X"] = static_cast<float>(rotXYZ.x);
	meta["localRot_Y"] = static_cast<float>(rotXYZ.y);
	meta["localRot_Z"] = static_cast<float>(rotXYZ.z);

	meta["localSca_X"] = static_cast<float>(localSca.x);
	meta["localSca_Y"] = static_cast<float>(localSca.y);
	meta["localSca_Z"] = static_cast<float>(localSca.z);
}

void ComponentTransform::InternalLoad(const Json& meta)
{
	localPos.x = static_cast<float>(meta["localPos_X"]);
	localPos.y = static_cast<float>(meta["localPos_Y"]);
	localPos.z = static_cast<float>(meta["localPos_Z"]);

	float3 rotation;
	rotation.x = static_cast<float>(meta["localRot_X"]);
	rotation.y = static_cast<float>(meta["localRot_Y"]);
	rotation.z = static_cast<float>(meta["localRot_Z"]);
	SetLocalRotation(rotation);

	localSca.x = static_cast<float>(meta["localSca_X"]);
	localSca.y = static_cast<float>(meta["localSca_Y"]);
	localSca.z = static_cast<float>(meta["localSca_Z"]);

	CalculateMatrices();
}

void ComponentTransform::CalculateMatrices()
{
	localMatrix = float4x4::FromTRS(localPos, localRot, localSca);

	const GameObject* parent = GetOwner()->GetParent();

	if (parent)
	{
		ComponentTransform* parentTransform = parent->GetComponentInternal<ComponentTransform>();

		// Set global matrix
		globalMatrix = parentTransform->GetGlobalMatrix().Mul(localMatrix);

		globalMatrix.Decompose(globalPos, globalRot, globalSca);
	}
	else
	{
		globalMatrix = localMatrix;
	}
}

void ComponentTransform::RecalculateLocalMatrix()
{
	globalMatrix = float4x4::FromTRS(globalPos, globalRot, globalSca);
	ComponentTransform* parentTransform = GetOwner()->GetParent()->GetComponentInternal<ComponentTransform>();
	localMatrix = parentTransform->GetGlobalMatrix().Inverted().Mul(globalMatrix);
	localMatrix.Decompose(localPos, localRot, localSca);
	rotXYZ = RadToDeg(localRot.ToEulerXYZ());
}

const float4x4 ComponentTransform::CalculatePaletteGlobalMatrix()
{
	localMatrix = float4x4::FromTRS(localPos, localRot, localSca);

	const GameObject* parent = GetOwner()->GetParent();
	const GameObject* root = GetOwner()->GetRootGO();

	if (parent != root)
	{
		ComponentTransform* parentTransform = parent->GetComponentInternal<ComponentTransform>();

		return parentTransform->CalculatePaletteGlobalMatrix().Mul(localMatrix);
	}
	else
	{
		return localMatrix;
	}
}

void ComponentTransform::UpdateTransformMatrices(bool notifyChanges)
{
	CalculateMatrices();

	if (notifyChanges)
	{
		for (Component* components : GetOwner()->GetComponents())
		{
			components->OnTransformChanged();
		}
	}

	if (GetOwner()->GetChildren().empty())
		return;

	for (GameObject* child : GetOwner()->GetChildren())
	{
		ComponentTransform* childTransform = child->GetComponentInternal<ComponentTransform>();

		if (childTransform)
		{
			childTransform->UpdateTransformMatrices(notifyChanges);
		}
	}
}

void ComponentTransform::CalculateLightTransformed(const ComponentLight* lightComponent,
												   bool translationModified,
												   bool rotationModified)
{
	Scene* loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	switch (lightComponent->GetLightType())
	{
		case LightType::DIRECTIONAL:
			if (rotationModified)
				loadedScene->RenderDirectionalLight();
			break;

		case LightType::POINT:
			if (translationModified)
			{
				loadedScene->UpdateScenePointLights();
				loadedScene->RenderPointLights();
			}
			break;

		case LightType::SPOT:
			if (translationModified || rotationModified)
			{
				loadedScene->UpdateSceneSpotLights();
				loadedScene->RenderSpotLights();
			}
			break;

		case LightType::AREA:
			if (translationModified || rotationModified)
			{
				loadedScene->UpdateSceneAreaLights();
				loadedScene->RenderAreaLights();
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

// CalculateLocalFromNewGlobal
// This will calculate the new local that the object needs to be iin order not to move with the change of father
//
void ComponentTransform::CalculateLocalFromNewGlobal(const ComponentTransform* newTransformFrom)
{
	const float4x4& nglobalMatrix = newTransformFrom->GetGlobalMatrix();
	float3 nPos, nSca;
	float4x4 nRot;
	nglobalMatrix.Decompose(nPos, nRot, nSca);

	localMatrix = newTransformFrom->GetGlobalMatrix().Inverted().Mul(globalMatrix);
	localMatrix.Decompose(localPos, localRot, localSca);
	rotXYZ = RadToDeg(localRot.ToEulerXYZ());
}