#include "ComponentTransform.h"
#include "ComponentLight.h"

#include "Application.h"
#include "Scene/Scene.h"

#include "Modules/ModuleDebugDraw.h"
#include "Modules/ModuleScene.h"

#include "Math/float3x3.h"

ComponentTransform::ComponentTransform(const bool active, GameObject* owner) :
	Component(ComponentType::TRANSFORM, active, owner, false),
	pos(float3::zero),
	rot(Quat::identity),
	sca(float3::one),
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

void ComponentTransform::InternalSave(Json& meta)
{
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

void ComponentTransform::InternalLoad(const Json& meta)
{
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
		ComponentTransform* parentTransform = parent->GetComponent<ComponentTransform>();

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
	ComponentTransform* parentTransform = GetOwner()->GetParent()->GetComponent<ComponentTransform>();
	localMatrix = parentTransform->GetGlobalMatrix().Inverted().Mul(globalMatrix);
	localMatrix.Decompose(pos, rot, sca);
	rotXYZ = RadToDeg(rot.ToEulerXYZ());
}

const float4x4 ComponentTransform::CalculatePaletteGlobalMatrix()
{
	localMatrix = float4x4::FromTRS(pos, rot, sca);

	const GameObject* parent = GetOwner()->GetParent();
	const GameObject* root = GetOwner()->GetRootGO();

	if (parent != root)
	{
		ComponentTransform* parentTransform = parent->GetComponent<ComponentTransform>();

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
		ComponentTransform* childTransform = 
			child->GetComponent<ComponentTransform>();

		if(childTransform)
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
	localMatrix.Decompose(pos, rot, sca);
	rotXYZ = RadToDeg(rot.ToEulerXYZ());
}