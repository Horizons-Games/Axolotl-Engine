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
	float hFov = App->camera->GetCamera()->GetHFOV();
	if (ImGui::SliderFloat("Horizontal FOV", &hFov, MIN_HFOV, MAX_HFOV, "%.0f", ImGuiSliderFlags_AlwaysClamp)) {
		App->camera->GetCamera()->SetHFOV(math::DegToRad(hFov));
	}

	float vFov = App->camera->GetCamera()->GetVFOV();
	if (ImGui::SliderFloat("Vertical FOV", &vFov, MIN_VFOV, MAX_VFOV, "%.0f", ImGuiSliderFlags_AlwaysClamp)) {
		App->camera->GetCamera()->SetVFOV(math::DegToRad(vFov));
	}

	float nearDistance = App->camera->GetCamera()->GetZNear();
	float farDistance = App->camera->GetCamera()->GetZFar();
	bool nearDistanceChanged = 
		ImGui::SliderFloat("Z near", &nearDistance, 0.1f, 200.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
	bool farDistanceChanged = 
		ImGui::SliderFloat("Z far", &farDistance, 500.f, 20000.f, "%0.f", ImGuiSliderFlags_AlwaysClamp);
	if (nearDistanceChanged || farDistanceChanged) {
		App->camera->GetCamera()->SetPlaneDistance(nearDistance, farDistance);
	}

	float movementSpeed = App->camera->GetCamera()->GetMoveSpeed();
	float rotationSpeed = App->camera->GetCamera()->GetRotationSpeed();

	if (ImGui::SliderFloat("Movement Speed", &movementSpeed,
		DEFAULT_MOVE_SPEED, DEFAULT_MOVE_SPEED * 100.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
		App->camera->GetCamera()->SetMoveSpeed(movementSpeed);
	if (ImGui::SliderFloat("Rotation Speed", &rotationSpeed,
		DEFAULT_ROTATION_SPEED, DEFAULT_ROTATION_SPEED * 10.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
		App->camera->GetCamera()->SetRotationSpeed(rotationSpeed);

	float kpPosition = App->camera->GetCamera()->GetKpPosition();
	float kpRotation = App->camera->GetCamera()->GetKpRotation();
	float interpolationDuration = App->camera->GetCamera()->GetInterpolationDuration();
	bool isFocusing = App->camera->GetCamera()->IsFocusing();
	bool isUsingProportionalController = App->camera->GetCamera()->IsUsingProportionalController();

	if (!isFocusing) 
	{
		if (ImGui::Checkbox("Use Proportional Controller", &isUsingProportionalController))
			App->camera->GetCamera()->SetUsingProportionalController(isUsingProportionalController);

		if (!isUsingProportionalController) 
		{
			if (ImGui::SliderFloat("Interpolation Duration", &interpolationDuration,
				0.05f, 2.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
				App->camera->GetCamera()->SetInterpolationDuration(interpolationDuration);
		}
		
	}

	if (isUsingProportionalController)
	{
		if (ImGui::SliderFloat("KP Position", &kpPosition,
			0.5f, 10.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
			App->camera->GetCamera()->SetKpPosition(kpPosition);
		if (ImGui::SliderFloat("KP Rotation", &kpRotation,
			0.05f, 2.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
			App->camera->GetCamera()->SetKpRotation(kpRotation);
	}
	

	static float4 color = App->renderer->GetBackgroundColor();
	ImGui::Text("Background Color");
	if (ImGui::ColorEdit3("MyColor##1", (float*)&color))
	{
		App->renderer->SetBackgroundColor(color);
	}
}
