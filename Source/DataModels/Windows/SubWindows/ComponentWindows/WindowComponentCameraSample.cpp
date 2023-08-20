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

		bool isSampleFixedEnabled = asCameraSample->GetFixedOffsetEnabled();
		float3 positionFixed = asCameraSample->GetFixedOffset();

		bool isCombatCameraEnabled = asCameraSample->GetCombatCameraEnabled();

		bool isSampleKpPositionEnabled = asCameraSample->GetKpPositionEnabled();
		float KpPosition = asCameraSample->GetKpPosition();
		bool isSampleKpRotationEnabled = asCameraSample->GetKpRotationEnabled();
		float KpRotation = asCameraSample->GetKpRotation();

		ImGui::Text("");

		if (ImGui::Checkbox("##Combat Camera Enabled", &isCombatCameraEnabled))
		{
			asCameraSample->SetCombatCameraEnabled(isCombatCameraEnabled);
		}

		ImGui::SameLine();
		ImGui::Text("Combat Camera Enabled");

		ImGui::Text("");

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

		if (ImGui::Checkbox("##Fixed Offset Enabled", &isSampleFixedEnabled))
		{
			asCameraSample->SetFixedOffsetEnabled(isSampleFixedEnabled);
		}

		ImGui::SameLine();
		ImGui::Text("Fixed Offset Enabled");

		if (isSampleFixedEnabled)
		{
			ImGui::Text("X:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::DragFloat("##XFixedOffset",
							 &positionFixed.x,
							 0.025f,
							 std::numeric_limits<float>::min(),
							 std::numeric_limits<float>::min());

			ImGui::SameLine();
			ImGui::Text("Y:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::DragFloat("##YFixedOffset",
							 &positionFixed.y,
							 0.025f,
							 std::numeric_limits<float>::min(),
							 std::numeric_limits<float>::min());

			ImGui::SameLine();
			ImGui::Text("Z:");
			ImGui::SameLine();
			ImGui::SetNextItemWidth(80.0f);
			ImGui::DragFloat("##ZFixedOffset",
							 &positionFixed.z,
							 0.025f,
							 std::numeric_limits<float>::min(),
							 std::numeric_limits<float>::min());

			asCameraSample->SetFixedOffset(positionFixed);
		}

		ImGui::Text("");

		if (ImGui::Checkbox("##Focus Offset Enabled", &isSampleFocusEnabled))
		{
			asCameraSample->SetFocusOffsetEnabled(isSampleFocusEnabled);
		}

		ImGui::SameLine();
		ImGui::Text("Focus Offset Enabled");

		if (isSampleFocusEnabled)
		{

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

		ImGui::Text("");

		if (ImGui::Checkbox("##Kp Position Enabled", &isSampleKpPositionEnabled))
		{
			asCameraSample->SetKpPositionEnabled(isSampleKpPositionEnabled);
		}

		ImGui::SameLine();
		ImGui::Text("Kp Position Enabled");

		if (isSampleKpPositionEnabled)
		{
			ImGui::SliderFloat("##KpPositionSample", &KpPosition, 0.0f, 10.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			asCameraSample->SetKpPosition(KpPosition);
		}

		if (ImGui::Checkbox("##Kp Rotation Enabled", &isSampleKpRotationEnabled))
		{
			asCameraSample->SetKpRotationEnabled(isSampleKpRotationEnabled);
		}

		ImGui::SameLine();
		ImGui::Text("Kp Rotation Enabled");

		if (isSampleKpRotationEnabled)
		{
			ImGui::SliderFloat("##KpRotationSample", &KpRotation, 0.0f, 5.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
			asCameraSample->SetKpRotation(KpRotation);
		}

		asCameraSample->SetRadius(influenceRadius);
		asCameraSample->SetOffset(positionOffset);
	}
}
