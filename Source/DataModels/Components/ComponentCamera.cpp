#pragma warning (disable: 26495)

#include "ComponentCamera.h"

#include "Application.h"
#include "ModuleDebugDraw.h"

#include "Math/float3x3.h"

#include "ComponentTransform.h"
#include "GameObject/GameObject.h"
#include "FileSystem/Json.h"

ComponentCamera::ComponentCamera(bool active, GameObject* owner)
	: Component(ComponentType::CAMERA, active, owner, false),
	frustumOffset(1.0f), drawFrustum(true), frustumMode(ECameraFrustumMode::normalFrustum),
	// PlaceHolder get position from component transform
	trans(static_cast<ComponentTransform*>(owner->GetComponent(ComponentType::TRANSFORM)))
{
	float aspectRatio = 16.f / 9.f;

	frustum.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);
	frustum.SetViewPlaneDistances(0.1f, 2000.f);
	frustum.SetHorizontalFovAndAspectRatio(math::DegToRad(90), aspectRatio);
	
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
	frustum.SetPos((float3) trans->GetGlobalPosition());

	float3x3 rotationMatrix = float3x3::FromQuat((Quat)trans->GetGlobalRotation());
	frustum.SetFront(rotationMatrix * float3::unitZ);
	frustum.SetUp(rotationMatrix * float3::unitY);


	if (frustumMode == ECameraFrustumMode::offsetFrustum) UpdateFrustumOffset();
}

void ComponentCamera::Draw()
{
	if(drawFrustum) App->debug->DrawFrustum(frustum);
}

void ComponentCamera::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
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
