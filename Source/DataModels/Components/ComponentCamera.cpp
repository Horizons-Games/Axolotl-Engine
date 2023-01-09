#include "ComponentCamera.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleDebugDraw.h"

#include "Math/float3x3.h"
#include "Math/Quat.h"


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
	float3 position = float3{ 10.f, 0.f, 0.f };

	frustum.SetPos(position);
	frustum.SetFront(-float3::unitZ);
	frustum.SetUp(float3::unitY);

	UpdateFrustumOffset();
}

ComponentCamera::~ComponentCamera()
{

}

void ComponentCamera::Init()
{
}

void ComponentCamera::Update()
{
	//Placeholder rotation to see the frustum
	Quat pitchQuat(frustum.WorldRight(), 0.f);
	Quat yawQuat(float3::unitY, 0.02f);

	float3x3 rotationMatrixX = float3x3::FromQuat(pitchQuat);
	float3x3 rotationMatrixY = float3x3::FromQuat(yawQuat);
	float3x3 rotationMatrix = rotationMatrixY * rotationMatrixX;

	vec oldFront = frustum.Front().Normalized();
	vec oldUp = frustum.Up().Normalized();

	frustum.SetFront(rotationMatrix.MulDir(oldFront));
	frustum.SetUp(rotationMatrix.MulDir(oldUp));

	if (frustumMode == ECameraFrustumMode::offsetFrustum) UpdateFrustumOffset();
	Draw();
}

void ComponentCamera::Draw()
{
	if(drawFrustum) App->debug->DrawFrustum(frustum);
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
