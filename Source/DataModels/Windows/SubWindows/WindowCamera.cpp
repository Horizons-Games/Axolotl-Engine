#include "StdAfx.h"

#include "WindowCamera.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"

WindowCamera::WindowCamera() : SubWindow("Camera")
{
}

WindowCamera::~WindowCamera()
{
}

void WindowCamera::DrawWindowContents()
{
	ModuleCamera* camera = App->GetModule<ModuleCamera>();

	float hFov = camera->GetCamera()->GetHFOV();
	if (ImGui::SliderFloat("Horizontal FOV", &hFov, MIN_HFOV, MAX_HFOV, "%.0f", ImGuiSliderFlags_AlwaysClamp))
	{
		camera->GetCamera()->SetHFOV(math::DegToRad(hFov));
	}

	float vFov = camera->GetCamera()->GetVFOV();
	if (ImGui::SliderFloat("Vertical FOV", &vFov, MIN_VFOV, MAX_VFOV, "%.0f", ImGuiSliderFlags_AlwaysClamp))
	{
		camera->GetCamera()->SetVFOV(math::DegToRad(vFov));
	}

	float nearDistance = camera->GetCamera()->GetZNear();
	float farDistance = camera->GetCamera()->GetZFar();
	bool nearDistanceChanged =
		ImGui::SliderFloat("Z near", &nearDistance, 0.1f, 200.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
	bool farDistanceChanged =
		ImGui::SliderFloat("Z far", &farDistance, 500.f, 20000.f, "%0.f", ImGuiSliderFlags_AlwaysClamp);
	if (nearDistanceChanged || farDistanceChanged)
	{
		camera->GetCamera()->SetPlaneDistance(nearDistance, farDistance);
	}

	float movementSpeed = camera->GetCamera()->GetMoveSpeed();
	float rotationSpeed = camera->GetCamera()->GetRotationSpeed();

	if (ImGui::SliderFloat("Movement Speed",
						   &movementSpeed,
						   DEFAULT_MOVE_SPEED,
						   DEFAULT_MOVE_SPEED * 100.f,
						   "%.2f",
						   ImGuiSliderFlags_AlwaysClamp))
		camera->GetCamera()->SetMoveSpeed(movementSpeed);
	if (ImGui::SliderFloat("Rotation Speed",
						   &rotationSpeed,
						   DEFAULT_ROTATION_SPEED,
						   DEFAULT_ROTATION_SPEED * 10.f,
						   "%.2f",
						   ImGuiSliderFlags_AlwaysClamp))
		camera->GetCamera()->SetRotationSpeed(rotationSpeed);

	float kpPosition = camera->GetCamera()->GetKpPosition();
	float kpRotation = camera->GetCamera()->GetKpRotation();
	float interpolationDuration = camera->GetCamera()->GetInterpolationDuration();
	bool isFocusing = camera->GetCamera()->IsFocusing();
	bool isUsingProportionalController = camera->GetCamera()->IsUsingProportionalController();

	if (!isFocusing)
	{
		if (ImGui::Checkbox("Use Proportional Controller", &isUsingProportionalController))
			camera->GetCamera()->SetUsingProportionalController(isUsingProportionalController);

		if (!isUsingProportionalController)
		{
			if (ImGui::SliderFloat(
					"Interpolation Duration", &interpolationDuration, 0.05f, 2.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
				camera->GetCamera()->SetInterpolationDuration(interpolationDuration);
		}
	}

	if (isUsingProportionalController)
	{
		if (ImGui::SliderFloat("KP Position", &kpPosition, 0.5f, 10.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
			camera->GetCamera()->SetKpPosition(kpPosition);
		if (ImGui::SliderFloat("KP Rotation", &kpRotation, 0.05f, 2.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
			camera->GetCamera()->SetKpRotation(kpRotation);
	}

	ModuleRender* render = App->GetModule<ModuleRender>();

	static float4 color = render->GetBackgroundColor();
	ImGui::Text("Background Color");
	if (ImGui::ColorEdit3("MyColor##1", (float*) &color))
	{
		render->SetBackgroundColor(color);
	}
}
