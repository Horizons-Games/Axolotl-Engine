#pragma warning (disable: 26495)

#include "ComponentTransform.h"
#include "ComponentLight.h"

#include "Application.h"
#include "Modules/ModuleScene.h"
#include "ModuleDebugDraw.h"

#include "GameObject/GameObject.h"
#include "Scene/Scene.h"
#include "FileSystem/Json.h"

#include "imgui.h"

#include "Math/float3x3.h"

ComponentTransform::ComponentTransform(const bool active, const std::shared_ptr<GameObject>& owner)
	: Component(ComponentType::TRANSFORM, active, owner, false)
{
}

void ComponentTransform::Update()
{
	// Empty for now
}

void ComponentTransform::Display()
{
	float3 translation = GetPosition();
	float3 rotation = GetRotationXYZ();
  	float3 scale = GetScale();

	float dragSpeed = 0.025f;

	bool translationModified = false;
	bool rotationModified = false;
	bool scaleModified = false;

	// The root must not be moved through the inspector
	if (App->scene->GetLoadedScene()->GetRoot() == this->GetOwner().lock())
		dragSpeed = 0.0f;

	if (ImGui::CollapsingHeader("TRANSFORM", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (ImGui::BeginTable("TransformTable", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Translation"); ImGui::SameLine();

			ImGui::TableNextColumn();
			ImGui::Text("x:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##XTrans", &translation.x, dragSpeed,
				std::numeric_limits<float>::min(), std::numeric_limits<float>::min()))
			{
				translationModified = true;
			}
			ImGui::PopStyleVar(); ImGui::SameLine();

			ImGui::Text("y:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##YTrans", &translation.y, dragSpeed,
				std::numeric_limits<float>::min(), std::numeric_limits<float>::min()))
			{
				translationModified = true;
			}
			ImGui::PopStyleVar(); ImGui::SameLine();

			ImGui::Text("z:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##ZTrans", &translation.z, dragSpeed,
				std::numeric_limits<float>::min(), std::numeric_limits<float>::min()))
			{
				translationModified = true;
			}
			ImGui::PopStyleVar();

			ImGui::TableNextColumn();
			ImGui::Text("Rotation"); ImGui::SameLine();

			ImGui::TableNextColumn();
			ImGui::Text("x:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##XRot", &rotation.x, dragSpeed,
				std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%0.3f"))
			{
				rotationModified = true;
			}
			ImGui::PopStyleVar(); ImGui::SameLine();

			ImGui::Text("y:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##YRot", &rotation.y, dragSpeed,
				std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%0.3f"))
			{
				rotationModified = true;
			}
			ImGui::PopStyleVar(); ImGui::SameLine();

			ImGui::Text("z:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##ZRot", &rotation.z, dragSpeed,
				std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%0.3f"))
			{
				rotationModified = true;
			}
			ImGui::PopStyleVar();

			ImGui::TableNextColumn();
			ImGui::Text("Scale"); ImGui::SameLine();

			ImGui::TableNextColumn();
			ImGui::Text("x:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##XScale", &scale.x, dragSpeed,
				0.0001f, std::numeric_limits<float>::max()))
			{
				scaleModified = true;
			}
			ImGui::PopStyleVar(); ImGui::SameLine();

			ImGui::Text("y:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if(ImGui::DragFloat("##YScale", &scale.y, dragSpeed,
				0.0001f, std::numeric_limits<float>::max()))
			{
				scaleModified = true;
			}
			ImGui::PopStyleVar(); ImGui::SameLine();

			ImGui::Text("z:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if(ImGui::DragFloat("##ZScale", &scale.z, dragSpeed,
				0.0001f, std::numeric_limits<float>::max()))
			{
				scaleModified = true;
			}
			ImGui::PopStyleVar();

			ImGui::EndTable();
		}
	}
	ImGui::Separator();

	if (App->scene->GetLoadedScene()->GetRoot() == this->GetOwner().lock())
	{
		SetPosition(float3::zero);
		SetRotation(float3::zero);
		SetScale(float3::one);
		return;
	}

	if (translationModified || rotationModified || scaleModified)
	{
		if (translationModified)
		{
			SetPosition(translation);
		}

		if (rotationModified)
		{
			SetRotation(rotation);
		}

		if (scaleModified)
		{
			if (scale.x <= 0) scale.x = 0.0001f;
			if (scale.y <= 0) scale.y = 0.0001f;
			if (scale.z <= 0) scale.z = 0.0001f;

			SetScale(scale);
		}

		UpdateTransformMatrices();
	}

	//Rendering lights if modified
	if (translationModified || rotationModified) 
	{
		std::shared_ptr<Component> comp = this->GetOwner().lock()->GetComponent(ComponentType::LIGHT);
		std::shared_ptr<ComponentLight> lightComp = std::static_pointer_cast<ComponentLight>(comp);

		if (lightComp)
		{
			CalculateLightTransformed(lightComp, translationModified, rotationModified);
		}
	}
}

void ComponentTransform::CalculateLightTransformed(const std::shared_ptr<ComponentLight>& lightComponent,
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

	CalculateMatrices();
}

void ComponentTransform::CalculateMatrices()
{
	//float4x4 localMatrix = float4x4::FromTRS((float3)GetPosition(), (float4x4)GetRotation(), (float3)GetScale());
	//SetLocalMatrix(localMatrix);

	std::shared_ptr<GameObject> parent = GetOwner().lock()->GetParent().lock();

	if (parent)
	{
		/*
		float3 parentPos, parentSca, localPos, localSca;
		float4x4 parentRot, localRot;

		std::shared_ptr<ComponentTransform> parentTransform =
			std::static_pointer_cast<ComponentTransform>(parent->GetComponent(ComponentType::TRANSFORM));

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
	}
}

const float3& ComponentTransform::GetGlobalPosition() const
{
	float3 globalPos, globalSca;
	float4x4 globalRot;
	globalMatrix.Decompose(globalPos, globalRot, globalSca);

	return globalPos;
}

const float4x4& ComponentTransform::GetGlobalRotation() const
{
	float3 globalPos, globalSca;
	float4x4 globalRot;
	globalMatrix.Decompose(globalPos, globalRot, globalSca);

	return globalRot;
}

const float3& ComponentTransform::GetGlobalScale() const
{
	float3 globalPos, globalSca;
	float4x4 globalRot;
	globalMatrix.Decompose(globalPos, globalRot, globalSca);

	return globalSca;
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