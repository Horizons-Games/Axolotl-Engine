#include "ComponentCamera.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleDebugDraw.h"

#include "Math/float3x3.h"
#include "Math/Quat.h"

#include "ComponentTransform.h"
#include "GameObject/GameObject.h"

#include "imgui.h"


ComponentCamera::ComponentCamera(bool active, GameObject* owner)
	: Component(ComponentType::CAMERA, active, owner)
{
	frustumOffset = 1;
	drawFrustum = true;
	frustumMode = ECameraFrustumMode::normalFrustum;

	float aspectRatio = 16.f / 9.f;

	frustum.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);
	frustum.SetViewPlaneDistances(0.1f, 10.f);
	frustum.SetHorizontalFovAndAspectRatio(math::DegToRad(90), aspectRatio);

	//Position PlaceHolder get position from component transform
	trans = (ComponentTransform*)owner->GetComponent(ComponentType::TRANSFORM);
	
	frustum.SetPos(trans->GetPosition());
	float3x3 rotationMatrix = float3x3::FromQuat(trans->GetRotation());
	frustum.SetFront(rotationMatrix * float3::unitZ);
	frustum.SetUp(rotationMatrix * float3::unitY);


	UpdateFrustumOffset();
}

ComponentCamera::~ComponentCamera()
{

}

void ComponentCamera::Update()
{
	// Get global position instead of local to set the camera frustum
	float3 globalPos, globalSca;
	Quat globalRot;
	trans->GetGlobalMatrix().Decompose(globalPos, globalRot, globalSca);

	frustum.SetPos(globalPos);

	float3x3 rotationMatrix = float3x3::FromQuat(globalRot);
	frustum.SetFront(rotationMatrix * float3::unitZ);
	frustum.SetUp(rotationMatrix * float3::unitY);


	if (frustumMode == ECameraFrustumMode::offsetFrustum) UpdateFrustumOffset();
}

void ComponentCamera::Draw()
{
	if(drawFrustum) App->debug->DrawFrustum(frustum);
}

void ComponentCamera::Display()
{

	const char* listbox_items[] = { "Basic Frustum", "Offset Frustum", "No Frustum" };


	ImGui::Text("CAMERA");
	ImGui::Dummy(ImVec2(0.0f, 2.5f));

	if (ImGui::BeginTable("CameraComponentTable", 2))
	{
		ImGui::TableNextColumn();
		ImGui::Text("Draw Frustum"); ImGui::SameLine();
		ImGui::Checkbox("", &drawFrustum);

		ImGui::ListBox("Frustum Mode\n(single select)", &frustumMode, listbox_items, IM_ARRAYSIZE(listbox_items), 3);
		ImGui::SliderFloat("Frustum Offset", &frustumOffset, -2.f, 2.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);

		ImGui::EndTable();
		ImGui::Separator();
	}
}

void ComponentCamera::UpdateFrustumOffset()
{
	math::Plane frustumPlanes[6];
	frustum.GetPlanes(frustumPlanes);

	for (int itPlanes = 0; itPlanes < 6; ++itPlanes)
	{
		math::Plane plane = frustumPlanes[itPlanes];
		plane.Translate(-frustumPlanes[itPlanes].normal * frustumOffset);
		offsetFrustumPlanes[itPlanes] = plane;
	}
}

bool ComponentCamera::IsInside(const OBB& obb)
{
	if (frustumMode == noFrustum) return false;
	if (frustumMode == offsetFrustum) return IsInsideOffset(obb);
	math::vec cornerPoints[8];
	math::Plane frustumPlanes[6];

	frustum.GetPlanes(frustumPlanes);
	obb.GetCornerPoints(cornerPoints);

	for (int itPlanes = 0; itPlanes < 6; ++itPlanes)
	{
		bool onPlane = false;
		for (int itPoints = 0; itPoints < 8; ++itPoints)
		{
			if (!frustumPlanes[itPlanes].IsOnPositiveSide(cornerPoints[itPoints]))
			{
				onPlane = true;
				break;
			}
		}
		if (!onPlane) return false;
	}

	return true;
}

bool ComponentCamera::IsInsideOffset(const OBB& obb)
{
	math::vec cornerPoints[8];
	obb.GetCornerPoints(cornerPoints);

	for (int itPlanes = 0; itPlanes < 6; ++itPlanes)
	{
		bool onPlane = false;
		for (int itPoints = 0; itPoints < 8; ++itPoints)
		{
			if (!offsetFrustumPlanes[itPlanes].IsOnPositiveSide(cornerPoints[itPoints]))
			{
				onPlane = true;
				break;
			}
		}
		if (!onPlane) return false;
	}

	return true;
}
