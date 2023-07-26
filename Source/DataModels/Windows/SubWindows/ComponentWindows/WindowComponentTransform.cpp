#include "StdAfx.h"

#include "WindowComponentTransform.h"

#include "Application.h"
#include "DataModels/Scene/Scene.h"
#include "ModuleScene.h"

#include "DataModels/Components/ComponentLight.h"
#include "DataModels/Components/ComponentTransform.h"

#include "ModuleScene.h"

WindowComponentTransform::WindowComponentTransform(ComponentTransform* component) :
	ComponentWindow("TRANSFORM", component),
	bbdraw(component->IsDrawBoundingBoxes()),
	uniformScale(component->IsUniformScale())
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
		if (ImGui::Checkbox("##Draw Bounding Box", &(bbdraw)))
		{
			asTransform->SetDrawBoundingBoxes(bbdraw);
			for (GameObject* child : asTransform->GetOwner()->GetChildren())
			{
				ComponentTransform* transform = child->GetComponentInternal<ComponentTransform>();
				transform->SetDrawBoundingBoxes(bbdraw);
			}
		}
		ImGui::SameLine();
		ImGui::Text("Draw Bounding Box");

		currentTranslation = asTransform->GetLocalPosition();
		currentRotation = asTransform->GetRotationXYZ();
		currentScale = asTransform->GetLocalScale();

		currentDragSpeed = 0.025f;

		translationModified = false;
		rotationModified = false;
		scaleModified = false;

		bool ownerIsRoot = App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot() == asTransform->GetOwner();

		// The root must not be moved through the inspector
		if (ownerIsRoot)
			currentDragSpeed = 0.0f;

		DrawTransformTable();

		if (ownerIsRoot)
		{
			asTransform->SetLocalPosition(float3::zero);
			asTransform->SetLocalRotation(Quat::identity);
			asTransform->SetLocalScale(float3::one);
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
		ImGui::Text("Translation");
		ImGui::SameLine();

		ImGui::TableNextColumn();
		ImGui::Text("x:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		if (ImGui::DragFloat("##XTrans",
							 &currentTranslation.x,
							 currentDragSpeed,
							 std::numeric_limits<float>::min(),
							 std::numeric_limits<float>::min()))
		{
			translationModified = true;
		}
		ImGui::PopStyleVar();
		ImGui::SameLine();

		ImGui::Text("y:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		if (ImGui::DragFloat("##YTrans",
							 &currentTranslation.y,
							 currentDragSpeed,
							 std::numeric_limits<float>::min(),
							 std::numeric_limits<float>::min()))
		{
			translationModified = true;
		}
		ImGui::PopStyleVar();
		ImGui::SameLine();

		ImGui::Text("z:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		if (ImGui::DragFloat("##ZTrans",
							 &currentTranslation.z,
							 currentDragSpeed,
							 std::numeric_limits<float>::min(),
							 std::numeric_limits<float>::min()))
		{
			translationModified = true;
		}
		ImGui::PopStyleVar();

		ImGui::TableNextColumn();
		ImGui::Text("Rotation");
		ImGui::SameLine();

		ImGui::TableNextColumn();
		ImGui::Text("x:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		if (ImGui::DragFloat("##XRot",
							 &currentRotation.x,
							 currentDragSpeed,
							 std::numeric_limits<float>::min(),
							 std::numeric_limits<float>::min(),
							 "%0.3f"))
		{
			rotationModified = true;
		}
		ImGui::PopStyleVar();
		ImGui::SameLine();

		ImGui::Text("y:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		if (ImGui::DragFloat("##YRot",
							 &currentRotation.y,
							 currentDragSpeed,
							 std::numeric_limits<float>::min(),
							 std::numeric_limits<float>::min(),
							 "%0.3f"))
		{
			rotationModified = true;
		}
		ImGui::PopStyleVar();
		ImGui::SameLine();

		ImGui::Text("z:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		if (ImGui::DragFloat("##ZRot",
							 &currentRotation.z,
							 currentDragSpeed,
							 std::numeric_limits<float>::min(),
							 std::numeric_limits<float>::min(),
							 "%0.3f"))
		{
			rotationModified = true;
		}
		ImGui::PopStyleVar();

		ImGui::TableNextColumn();
		ImGui::Text("Scale");
		ImGui::SameLine();

		ImGui::TableNextColumn();
		ImGui::Text("x:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		if (ImGui::DragFloat("##XScale", &currentScale.x, currentDragSpeed, 0.0001f, std::numeric_limits<float>::max()))
		{
			scaleModified = true;
			modifiedScaleAxis = Axis::X;
		}
		ImGui::PopStyleVar();
		ImGui::SameLine();

		ImGui::Text("y:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		if (ImGui::DragFloat("##YScale", &currentScale.y, currentDragSpeed, 0.0001f, std::numeric_limits<float>::max()))
		{
			scaleModified = true;
			modifiedScaleAxis = Axis::Y;
		}
		ImGui::PopStyleVar();
		ImGui::SameLine();

		ImGui::Text("z:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		if (ImGui::DragFloat("##ZScale", &currentScale.z, currentDragSpeed, 0.0001f, std::numeric_limits<float>::max()))
		{
			scaleModified = true;
			modifiedScaleAxis = Axis::Z;
		}
		ImGui::PopStyleVar();
		ImGui::TableNextColumn();
		ImGui::Checkbox("", &uniformScale);
		ImGui::SameLine();
		ImGui::Text("Maintain scale");
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
			asTransform->SetLocalPosition(currentTranslation);
		}

		if (rotationModified)
		{
			asTransform->SetLocalRotation(currentRotation);
		}

		if (scaleModified)
		{
			if (uniformScale)
			{
				asTransform->SetUniformScale(currentScale, modifiedScaleAxis);
			}
			else
			{
				asTransform->SetLocalScale(currentScale);
			}
		}

		if (scaleModified || rotationModified || translationModified)
		{
			asTransform->UpdateTransformMatrices();
		}
	}
}

void WindowComponentTransform::UpdateLights()
{
	ComponentTransform* asTransform = static_cast<ComponentTransform*>(component);

	if (asTransform)
	{
		// Rendering lights if modified
		if (translationModified || rotationModified)
		{
			const ComponentLight* lightComp = asTransform->GetOwner()->GetComponentInternal<ComponentLight>();

			if (lightComp)
			{
				asTransform->CalculateLightTransformed(lightComp, translationModified, rotationModified);
			}
		}
	}
}
