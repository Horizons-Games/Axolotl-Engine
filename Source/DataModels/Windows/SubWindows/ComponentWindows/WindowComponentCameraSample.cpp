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

		ImGui::Text("");
		ImGui::Text("Position Offset");
		ImGui::Text("X:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::DragFloat("##XOffset",
						 &positionOffset.x,
						 0.025f,
						 std::numeric_limits<float>::min(),
						 std::numeric_limits<float>::min());

		ImGui::SameLine();
		ImGui::Text("Y:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::DragFloat("##YOffset",
						 &positionOffset.y,
						 0.025f,
						 std::numeric_limits<float>::min(),
						 std::numeric_limits<float>::min());

		ImGui::SameLine();
		ImGui::Text("Z:");
		ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::DragFloat("##ZOffset",
						 &positionOffset.z,
						 0.025f,
						 std::numeric_limits<float>::min(),
						 std::numeric_limits<float>::min());

		
		ImGui::Text("");

		if (ImGui::Checkbox("##Focus Offset Enabled", &isSampleFocusEnabled))
		{
			asCameraSample->SetFocusOffsetEnabled(isSampleFocusEnabled);
		}

		ImGui::SameLine();
		ImGui::Text("Focus Offset Enabled");

		if (isSampleFocusEnabled)
		{

			ImGui::Text("Focus Offset");
			ImGui::Text("");
			ImGui::Text("Horizontal:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::DragFloat("##XFocusOffset",
							 &focusOffset.x,
							 0.025f,
							 std::numeric_limits<float>::min(),
							 std::numeric_limits<float>::min());

			ImGui::SameLine();
			ImGui::Text("Vertical:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::DragFloat("##YFocusOffset",
							 &focusOffset.y,
							 0.025f,
							 std::numeric_limits<float>::min(),
							 std::numeric_limits<float>::min());


			asCameraSample->SetFocusOffset(focusOffset);
		}

		asCameraSample->SetRadius(influenceRadius);
		asCameraSample->SetOffset(positionOffset);
		
		
	}
}
