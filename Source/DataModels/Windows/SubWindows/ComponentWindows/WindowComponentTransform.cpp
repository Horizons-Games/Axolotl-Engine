#include "WindowComponentTransform.h"

#include "Application.h"
#include "ModuleScene.h"
#include "DataModels/Scene/Scene.h"

#include "DataModels/Components/ComponentTransform.h"
#include "DataModels/Components/ComponentLight.h"

WindowComponentTransform::WindowComponentTransform(ComponentTransform* component) :
	ComponentWindow("TRANSFORM", component)
{
}

WindowComponentTransform::~WindowComponentTransform()
{
}

void WindowComponentTransform::DrawWindowContents()
{
	ComponentTransform* asTransform = static_cast<ComponentTransform*>(component);

	if (asTransform)
	{
		currentTranslation = asTransform->GetPosition();
		currentRotation = asTransform->GetRotationXYZ();
		currentScale = asTransform->GetScale();

		currentDragSpeed = 0.025f;

		translationModified = false;
		rotationModified = false;
		scaleModified = false;

		bool ownerIsRoot = App->scene->GetLoadedScene()->GetRoot() == asTransform->GetOwner();

		// The root must not be moved through the inspector
		if (ownerIsRoot)
			currentDragSpeed = 0.0f;

		DrawTransformTable();

		if (ownerIsRoot)
		{
			asTransform->SetPosition(float3::zero);
			asTransform->SetRotation(Quat::identity);
			asTransform->SetScale(float3::one);
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
	ComponentTransform* asTransform = static_cast<ComponentTransform*>(component);

	if (asTransform)
	{
		if (translationModified)
		{
			asTransform->SetPosition(currentTranslation);
		}

		if (rotationModified)
		{
			asTransform->SetRotation(currentRotation);
		}

		if (scaleModified)
		{
			if (currentScale.x <= 0)
			{
				currentScale.x = 0.0001f;
			}
			
			if (currentScale.y <= 0)
			{
				currentScale.y = 0.0001f;
			}
			
			if (currentScale.z <= 0) 
			{ 
				currentScale.z = 0.0001f; 
			}

			asTransform->SetScale(currentScale);
		}

		asTransform->CalculateLocalMatrix();
		asTransform->CalculateGlobalMatrix();
	}
}

void WindowComponentTransform::UpdateLights()
{
	ComponentTransform* asTransform = static_cast<ComponentTransform*>(component);

	if (asTransform)
	{
		//Rendering lights if modified
		if (translationModified || rotationModified)
		{
			Component* comp = asTransform->GetOwner()->GetComponent(ComponentType::LIGHT);
			ComponentLight* lightComp = static_cast<ComponentLight*>(comp);

			if (lightComp)
			{
				asTransform->CalculateLightTransformed(lightComp, translationModified, rotationModified);
			}
		}
	}
}
