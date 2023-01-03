#include "ComponentCamera.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleDebugDraw.h"


ComponentCamera::ComponentCamera(bool active, GameObject* owner)
	: Component(ComponentType::CAMERA, active, owner)
{
	frustumOffset = 1;
	drawFrustum = true;
	frustumMode = ECameraFrustumMode::normalFrustum;

	//Frustum Init
	int w, h;

	SDL_GetWindowSize(App->window->GetWindow(), &w, &h);
	float aspectRatio = float(w) / h;

	frustum.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);
	frustum.SetViewPlaneDistances(0.1f, 100.0f);
	frustum.SetHorizontalFovAndAspectRatio(math::DegToRad(90), aspectRatio);

	//Position PlaceHolder get position from component transform
	float3 position = float3{ 0,0,0 };

	frustum.SetPos(position);
	frustum.SetFront(-float3::unitZ);
	frustum.SetUp(float3::unitY);

	UpdateFrustumOffset();
}

void ComponentCamera::Update()
{
	//TODO:: Make placeholder rotation to see the frustum

	if (frustumMode == ECameraFrustumMode::offsetFrustum) UpdateFrustumOffset();
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