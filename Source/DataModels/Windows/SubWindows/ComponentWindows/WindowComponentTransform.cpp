#include "WindowComponentTransform.h"

#include "Application.h"
#include "ModuleScene.h"
#include "DataModels/Scene/Scene.h"
#include "DataModels/GameObject/GameObject.h"

#include "DataModels/Components/ComponentTransform.h"

WindowComponentTransform::WindowComponentTransform(const std::weak_ptr<ComponentTransform>& component) :
	ComponentWindow("TRANSFORM", component)
{
}

void WindowComponentTransform::DrawWindowContents()
{
	std::shared_ptr<ComponentTransform> asSharedTransform =
		std::dynamic_pointer_cast<ComponentTransform>(this->component.lock());

	if (asSharedTransform)
	{
		float3 translation = asSharedTransform->GetPosition();
		float3 rotation = asSharedTransform->GetRotationXYZ();
		float3 scale = asSharedTransform->GetScale();

		float dragSpeed = 0.025f;

		bool translationModified = false;
		bool rotationModified = false;
		bool scaleModified = false;

		bool ownerIsRoot = App->scene->GetLoadedScene()->GetRoot() == asSharedTransform->GetOwner().lock();

		// The root must not be moved through the inspector
		if (ownerIsRoot)
			dragSpeed = 0.0f;

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
			if (ImGui::DragFloat("##YScale", &scale.y, dragSpeed,
				0.0001f, std::numeric_limits<float>::max()))
			{
				scaleModified = true;
			}
			ImGui::PopStyleVar(); ImGui::SameLine();

			ImGui::Text("z:"); ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			if (ImGui::DragFloat("##ZScale", &scale.z, dragSpeed,
				0.0001f, std::numeric_limits<float>::max()))
			{
				scaleModified = true;
			}
			ImGui::PopStyleVar();

			ImGui::EndTable();
		}

		if (ownerIsRoot)
		{
			asSharedTransform->SetPosition(float3::zero);
			asSharedTransform->SetRotation(Quat::identity);
			asSharedTransform->SetScale(float3::one);
			return;
		}

		if (translationModified)
		{
			asSharedTransform->SetPosition(translation);
		}

		if (rotationModified)
		{
			asSharedTransform->SetRotation(rotation);
		}

		if (scaleModified)
		{
			if (scale.x <= 0) scale.x = 0.0001f;
			if (scale.y <= 0) scale.y = 0.0001f;
			if (scale.z <= 0) scale.z = 0.0001f;

			asSharedTransform->SetScale(scale);
		}

		asSharedTransform->CalculateLocalMatrix();
		asSharedTransform->CalculateGlobalMatrix();

		//Rendering lights if modified
		if (translationModified || rotationModified)
		{
			std::shared_ptr<Component> comp = asSharedTransform->GetOwner().lock()->GetComponent(ComponentType::LIGHT);
			std::shared_ptr<ComponentLight> lightComp = std::static_pointer_cast<ComponentLight>(comp);

			if (lightComp)
			{
				asSharedTransform->CalculateLightTransformed(lightComp, translationModified, rotationModified);
			}
		}
	}
}

void WindowComponentTransform::DrawTransformTable()
{
}
