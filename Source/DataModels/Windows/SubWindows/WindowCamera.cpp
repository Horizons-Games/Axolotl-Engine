#include "WindowCamera.h"

#include "imgui.h"

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
	float hFov = App->engineCamera->GetCamera()->GetHFOV();
	if (ImGui::SliderFloat("Horizontal FOV", &hFov, MIN_HFOV, MAX_HFOV, "%.0f", ImGuiSliderFlags_AlwaysClamp)) {
		App->engineCamera->GetCamera()->SetHFOV(math::DegToRad(hFov));
	}

	float vFov = App->engineCamera->GetCamera()->GetVFOV();
	if (ImGui::SliderFloat("Vertical FOV", &vFov, MIN_VFOV, MAX_VFOV, "%.0f", ImGuiSliderFlags_AlwaysClamp)) {
		App->engineCamera->GetCamera()->SetVFOV(math::DegToRad(vFov));
	}

	float nearDistance = App->engineCamera->GetCamera()->GetZNear();
	float farDistance = App->engineCamera->GetCamera()->GetZFar();
	bool nearDistanceChanged = 
		ImGui::SliderFloat("Z near", &nearDistance, 0.1f, 200.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
	bool farDistanceChanged = 
		ImGui::SliderFloat("Z far", &farDistance, 500.f, 20000.f, "%0.f", ImGuiSliderFlags_AlwaysClamp);
	if (nearDistanceChanged || farDistanceChanged) {
		App->engineCamera->GetCamera()->SetPlaneDistance(nearDistance, farDistance);
	}

	float movementSpeed = App->engineCamera->GetCamera()->GetMoveSpeed();
	float rotationSpeed = App->engineCamera->GetCamera()->GetRotationSpeed();

	if (ImGui::SliderFloat("Movement Speed", &movementSpeed,
		DEFAULT_MOVE_SPEED, DEFAULT_MOVE_SPEED * 100.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
		App->engineCamera->GetCamera()->SetMoveSpeed(movementSpeed);
	if (ImGui::SliderFloat("Rotation Speed (keyboard)", &rotationSpeed,
		DEFAULT_ROTATION_SPEED, DEFAULT_ROTATION_SPEED * 10.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
		App->engineCamera->GetCamera()->SetRotationSpeed(rotationSpeed);

	static float4 color = App->renderer->GetBackgroundColor();
	ImGui::Text("Background Color");
	if (ImGui::ColorEdit3("MyColor##1", (float*)&color))
		App->renderer->SetBackgroundColor(color);
}
