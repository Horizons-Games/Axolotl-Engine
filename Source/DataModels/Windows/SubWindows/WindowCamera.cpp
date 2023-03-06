#include "WindowCamera.h"

#include "Application.h"
#include "ModuleEngineCamera.h"
#include "ModuleRender.h"

WindowCamera::WindowCamera() : SubWindow("Camera")
{
}

WindowCamera::~WindowCamera()
{
}

void WindowCamera::DrawWindowContents()
{
	ModuleEngineCamera* engineCamera = App->GetModuleEngineCamera();
	
	float hFov = engineCamera->GetHFOV();
	if (ImGui::SliderFloat("Horizontal FOV", &hFov, MIN_HFOV, MAX_HFOV, "%.0f", ImGuiSliderFlags_AlwaysClamp)) 
	{
		engineCamera->SetHFOV(math::DegToRad(hFov));
	}

	float vFov = engineCamera->GetVFOV();
	if (ImGui::SliderFloat("Vertical FOV", &vFov, MIN_VFOV, MAX_VFOV, "%.0f", ImGuiSliderFlags_AlwaysClamp)) 
	{
		engineCamera->SetVFOV(math::DegToRad(vFov));
	}

	float nearDistance = engineCamera->GetZNear();
	float farDistance = engineCamera->GetZFar();
	bool nearDistanceChanged = 
		ImGui::SliderFloat("Z near", &nearDistance, 0.1f, 200.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
	bool farDistanceChanged = 
		ImGui::SliderFloat("Z far", &farDistance, 500.f, 20000.f, "%0.f", ImGuiSliderFlags_AlwaysClamp);
	if (nearDistanceChanged || farDistanceChanged) 
	{
		engineCamera->SetPlaneDistance(nearDistance, farDistance);
	}

	float movementSpeed = engineCamera->GetMoveSpeed();
	float rotationSpeed = engineCamera->GetRotationSpeed();

	if (ImGui::SliderFloat("Movement Speed", &movementSpeed,
		DEFAULT_MOVE_SPEED, DEFAULT_MOVE_SPEED * 100.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
	{
		engineCamera->SetMoveSpeed(movementSpeed);
	}
	
	if (ImGui::SliderFloat("Rotation Speed", &rotationSpeed,
		DEFAULT_ROTATION_SPEED, DEFAULT_ROTATION_SPEED * 10.f, "%.2f", ImGuiSliderFlags_AlwaysClamp))
	{
		engineCamera->SetRotationSpeed(rotationSpeed);
	}

	color = App->GetModuleRender()->GetBackgroundColor();
	ImGui::Text("Background Color");
	if (ImGui::ColorEdit3("MyColor##1", (float*)&color))
	{
		App->GetModuleRender()->SetBackgroundColor(color);
	}
}
