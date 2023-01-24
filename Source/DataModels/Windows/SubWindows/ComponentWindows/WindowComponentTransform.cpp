#include "WindowComponentTransform.h"

#include "Application.h"
#include "ModuleScene.h"
#include "DataModels/Scene/Scene.h"
#include "DataModels/GameObject/GameObject.h"

#include "DataModels/Components/ComponentTransform.h"
#include "DataModels/Components/ComponentLight.h"

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
		currentTranslation = asSharedTransform->GetPosition();
		currentRotation = asSharedTransform->GetRotationXYZ();
		currentScale = asSharedTransform->GetScale();

		currentDragSpeed = 0.025f;

		translationModified = false;
		rotationModified = false;
		scaleModified = false;

		bool ownerIsRoot = App->scene->GetLoadedScene()->GetRoot() == asSharedTransform->GetOwner().lock();

		// The root must not be moved through the inspector
		if (ownerIsRoot)
			currentDragSpeed = 0.0f;

		DrawTransformTable();

		if (ownerIsRoot)
		{
			asSharedTransform->SetPosition(float3::zero);
			asSharedTransform->SetRotation(Quat::identity);
			asSharedTransform->SetScale(float3::one);
			return;
		}

		UpdateComponentTransform();

		UpdateLights();
	}
}

void WindowComponentTransform::DrawTransformTable()
{
	if (ImGui::BeginTable("TransformTable", 2))
	{
		ImGui::TableNextColumn();
		ImGui::Text("Translation"); ImGui::SameLine();

		ImGui::TableNextColumn();
		ImGui::Text("x:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		if (ImGui::DragFloat("##XTrans", &currentTranslation.x, currentDragSpeed,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min()))
		{
			translationModified = true;
		}
		ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		if (ImGui::DragFloat("##YTrans", &currentTranslation.y, currentDragSpeed,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min()))
		{
			translationModified = true;
		}
		ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		if (ImGui::DragFloat("##ZTrans", &currentTranslation.z, currentDragSpeed,
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
		if (ImGui::DragFloat("##XRot", &currentRotation.x, currentDragSpeed,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%0.3f"))
		{
			rotationModified = true;
		}
		ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		if (ImGui::DragFloat("##YRot", &currentRotation.y, currentDragSpeed,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%0.3f"))
		{
			rotationModified = true;
		}
		ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		if (ImGui::DragFloat("##ZRot", &currentRotation.z, currentDragSpeed,
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
		if (ImGui::DragFloat("##XScale", &currentScale.x, currentDragSpeed,
			0.0001f, std::numeric_limits<float>::max()))
		{
			scaleModified = true;
		}
		ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		if (ImGui::DragFloat("##YScale", &currentScale.y, currentDragSpeed,
			0.0001f, std::numeric_limits<float>::max()))
		{
			scaleModified = true;
		}
		ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		if (ImGui::DragFloat("##ZScale", &currentScale.z, currentDragSpeed,
			0.0001f, std::numeric_limits<float>::max()))
		{
			scaleModified = true;
		}
		ImGui::PopStyleVar();

		ImGui::EndTable();
	}
}

void WindowComponentTransform::UpdateComponentTransform()
{
	std::shared_ptr<ComponentTransform> asSharedTransform =
		std::dynamic_pointer_cast<ComponentTransform>(this->component.lock());

	if (asSharedTransform)
	{
		if (translationModified)
		{
			asSharedTransform->SetPosition(currentTranslation);
		}

		if (rotationModified)
		{
			asSharedTransform->SetRotation(currentRotation);
		}

		if (scaleModified)
		{
			if (currentScale.x <= 0) currentScale.x = 0.0001f;
			if (currentScale.y <= 0) currentScale.y = 0.0001f;
			if (currentScale.z <= 0) currentScale.z = 0.0001f;

			asSharedTransform->SetScale(currentScale);
		}

		asSharedTransform->CalculateLocalMatrix();
		asSharedTransform->CalculateGlobalMatrix();
	}
}

void WindowComponentTransform::UpdateLights()
{
	std::shared_ptr<ComponentTransform> asSharedTransform =
		std::dynamic_pointer_cast<ComponentTransform>(this->component.lock());

	if (asSharedTransform)
	{
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
