#include "ComponentCamera.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleDebugDraw.h"

#include "Math/float3x3.h"
#include "Math/Quat.h"

#include "ComponentTransform.h"
#include "GameObject/GameObject.h"
#include "FileSystem/Json.h"

#include "imgui.h"


ComponentCamera::ComponentCamera(bool active, GameObject* owner)
	: Component(ComponentType::CAMERA, active, owner, false)
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
	frustum.SetPos((float3)trans->GetGlobalPosition());

	float3x3 rotationMatrix = float3x3::FromQuat((Quat)trans->GetGlobalRotation());
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

void ComponentCamera::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["owner"] = (GameObject*)owner;
	meta["removed"] = (bool)canBeRemoved;

	meta["frustumOfset"] = (float)frustumOffset;
	meta["drawFrustum"] = (bool)drawFrustum;
	meta["frustumMode"] = (int)frustumMode;
}

void ComponentCamera::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	//owner = (GameObject*) meta["owner"];
	canBeRemoved = (bool)meta["removed"];

	frustumOffset = (float)meta["frustumOfset"];
	drawFrustum = (bool)meta["drawFrustum"];
	frustumMode = (int)meta["frustumMode"];
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
