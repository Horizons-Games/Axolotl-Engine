#include "StdAfx.h"

#include "WindowComponentCamera.h"

#include "Application.h"
#include "Modules/ModuleCamera.h"

#include "Camera/CameraGameObject.h"
#include "DataModels/Components/ComponentCamera.h"

WindowComponentCamera::WindowComponentCamera(ComponentCamera* component) : ComponentWindow("CAMERA", component)
{
}

WindowComponentCamera::~WindowComponentCamera()
{
}

void WindowComponentCamera::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ComponentCamera* asCamera = static_cast<ComponentCamera*>(component);

	if (asCamera)
	{
		ImGui::Text("");

		const char* listbox_items[] = { "Basic Frustum", "Offset Frustum", "No Frustum" };

		bool drawFrustum = asCamera->GetCamera()->IsDrawFrustum();
		EFrustumMode frustumMode = asCamera->GetCamera()->GetFrustumMode();
		int frustumModeAsNumber = static_cast<int>(frustumMode);
		float frustumOffset = asCamera->GetCamera()->GetFrustumOffset();
		float kpPosition = asCamera->GetKpPosition();
		float kpRotation = asCamera->GetKpRotation();

		float hFov = asCamera->GetCamera()->GetHFOV();
		if (ImGui::SliderFloat("Horizontal FOV", &hFov, MIN_HFOV, MAX_HFOV, "%.0f", ImGuiSliderFlags_AlwaysClamp))
		{
			asCamera->GetCamera()->SetHFOV(math::DegToRad(hFov));
		}

		float vFov = asCamera->GetCamera()->GetVFOV();
		if (ImGui::SliderFloat("Vertical FOV", &vFov, MIN_VFOV, MAX_VFOV, "%.0f", ImGuiSliderFlags_AlwaysClamp))
		{
			asCamera->GetCamera()->SetVFOV(math::DegToRad(vFov));
		}

		float nearDistance = asCamera->GetCamera()->GetZNear();
		float farDistance = asCamera->GetCamera()->GetZFar();
		bool nearDistanceChanged =
			ImGui::SliderFloat("Z near", &nearDistance, 0.1f, 200.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
		bool farDistanceChanged =
			ImGui::SliderFloat("Z far", &farDistance, 50.f, 20000.f, "%0.f", ImGuiSliderFlags_AlwaysClamp);
		if (nearDistanceChanged || farDistanceChanged)
		{
			asCamera->GetCamera()->SetPlaneDistance(nearDistance, farDistance);
		}

		bool isFrustumChecked = asCamera->IsFrustumChecked();

		ImGui::Text("Draw Frustum");
		ImGui::SameLine();
		ImGui::Checkbox("##Draw Frustum", &drawFrustum);

		ImGui::Text("Use this Frustum");
		ImGui::SameLine();
		ImGui::Checkbox("##Use this Frustum", &isFrustumChecked);

		ImGui::ListBox(
			"Frustum Mode\n(single select)", &frustumModeAsNumber, listbox_items, IM_ARRAYSIZE(listbox_items), 3);
		ImGui::SliderFloat("Frustum Offset", &frustumOffset, -2.f, 2.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);

		ImGui::SliderFloat("Kp Position", &kpPosition, 1.0f, 10.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::SliderFloat("Kp Rotation", &kpRotation, 1.0f, 5.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);

		asCamera->GetCamera()->SetIsDrawFrustum(drawFrustum);
		EFrustumMode newFrustumMode = static_cast<EFrustumMode>(frustumModeAsNumber);
		asCamera->GetCamera()->SetFrustumMode(newFrustumMode);
		asCamera->GetCamera()->SetFrustumOffset(frustumOffset);

		if (isFrustumChecked)
		{
			ModuleCamera* moduleCamera = App->GetModule<ModuleCamera>();
			moduleCamera->SetFrustumCheckedCamera(asCamera);
		}

		else
		{
			asCamera->SetFrustumChecked(false);
		}
		

		asCamera->SetKpPosition(kpPosition);
		asCamera->SetKpRotation(kpRotation);
	}
}
