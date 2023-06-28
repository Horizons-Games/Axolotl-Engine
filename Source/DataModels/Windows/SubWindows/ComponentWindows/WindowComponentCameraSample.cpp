#include "StdAfx.h"
#include "WindowComponentCameraSample.h"

#include "Camera/CameraGameObject.h"
#include "DataModels/Components/ComponentCameraSample.h"

WindowComponentCameraSample::WindowComponentCameraSample(ComponentCameraSample* component) :
	ComponentWindow("CAMERA SAMPLE", component)
{
}

WindowComponentCameraSample::~WindowComponentCameraSample()
{
}

void WindowComponentCameraSample::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ComponentCameraSample* asCameraSample = static_cast<ComponentCameraSample*>(component);

	if (asCameraSample)
	{
		ImGui::Text("");

		float influenceRadius = asCameraSample->GetRadius();
		float3 positionOffset = asCameraSample->GetOffset();

		bool isSampleFocusEnabled = asCameraSample->GetFocusOffsetEnabled();
		float2 focusOffset = asCameraSample->GetFocusOffset();

		ImGui::SliderFloat("Influence Radius", &influenceRadius, 0.0f, 30.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

		if (ImGui::BeginTable("TransformTable", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Position Offset");
			ImGui::SameLine();

			ImGui::TableNextColumn();
			ImGui::Text("x:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			ImGui::DragFloat("##XOffset",
							 &positionOffset.x,
							 0.025f,
							 std::numeric_limits<float>::min(),
							 std::numeric_limits<float>::min());
			
			ImGui::PopStyleVar();
			ImGui::SameLine();

			ImGui::Text("y:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			ImGui::DragFloat("##YOffset",
							 &positionOffset.y,
							 0.025f,
							 std::numeric_limits<float>::min(),
							 std::numeric_limits<float>::min());
			ImGui::PopStyleVar();
			ImGui::SameLine();

			ImGui::Text("z:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
			ImGui::DragFloat("##ZOffset",
							 &positionOffset.z,
							 0.025f,
							 std::numeric_limits<float>::min(),
							 std::numeric_limits<float>::min());
			ImGui::PopStyleVar();
			ImGui::EndTable();
		}

		if (ImGui::Checkbox("##Focus Offset Enabled", &isSampleFocusEnabled))
		{
			asCameraSample->SetFocusOffsetEnabled(isSampleFocusEnabled);
		}

		ImGui::SameLine();
		ImGui::Text("Focus Offset Enabled");

		if (isSampleFocusEnabled)
		{
			if (ImGui::BeginTable("TransformTable", 2))
			{
				ImGui::TableNextColumn();
				ImGui::Text("Focus Offset");
				ImGui::SameLine();

				ImGui::TableNextColumn();
				ImGui::Text("Horizontal:");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(80.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
				ImGui::DragFloat("##XFocusOffset",
								 &focusOffset.x,
								 0.025f,
								 std::numeric_limits<float>::min(),
								 std::numeric_limits<float>::min());

				ImGui::PopStyleVar();
				ImGui::SameLine();

				ImGui::Text("Vertical:");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(80.0f);
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
				ImGui::DragFloat("##YFocusOffset",
								 &focusOffset.y,
								 0.025f,
								 std::numeric_limits<float>::min(),
								 std::numeric_limits<float>::min());
				ImGui::PopStyleVar();

				ImGui::EndTable();
			}

			asCameraSample->SetFocusOffset(focusOffset);
		}

		asCameraSample->SetRadius(influenceRadius);
		asCameraSample->SetOffset(positionOffset);
		
		
	}
}
