#include "StdAfx.h"
#include "ComponentPlanarReflection.h"

#include "Application.h"
#include "ModuleScene.h"

#include "ComponentTransform.h"

#include "Geometry/Frustum.h"

#include "FileSystem/Json.h"

#include "debugdraw.h"

ComponentPlanarReflection::ComponentPlanarReflection(GameObject* parent) : 
	ComponentLight(LightType::PLANAR_REFLECTION, parent, true),	frameBuffer(0), depth(0), reflectionTex(0), 
	planeNormal(0, 1, 0), originScaling({ 0.5f, 0.5f, 0.5f })
{
	if (GetOwner()->HasComponent<ComponentTransform>())
	{
		influenceAABB = { GetPosition() + float3(-5, -0.5f, -5), GetPosition() + float3(5, 0.5f, 5) };
	}
}

ComponentPlanarReflection::~ComponentPlanarReflection()
{
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteRenderbuffers(1, &depth);

	glDeleteTextures(1, &reflectionTex);
}

void ComponentPlanarReflection::Draw() const
{
	bool canDrawPlanarReflection =
#ifdef ENGINE
		IsEnabled() && GetOwner() == App->GetModule<ModuleScene>()->GetSelectedGameObject();
#else
		false;
#endif // ENGINE

	if (!canDrawPlanarReflection)
	{
		return;
	}

	dd::aabb(influenceAABB.minPoint, influenceAABB.maxPoint, dd::colors::Blue);
}

void ComponentPlanarReflection::OnTransformChanged()
{
	// Normal rotation
	planeNormal = GetRotation() * float3(0, 1, 0);

	// Pos translation
	float3 halfsize = influenceAABB.HalfSize();

	influenceAABB.minPoint = (GetPosition() - halfsize);
	influenceAABB.maxPoint = (GetPosition() + halfsize);

	AXO_TODO("Recalculate all the meshes that are inside the aabb");
}

void ComponentPlanarReflection::InitBuffer(unsigned width, unsigned height)
{
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glGenRenderbuffers(1, &depth);
	glBindRenderbuffer(GL_RENDERBUFFER, depth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth);

	glGenTextures(1, &reflectionTex);
	glBindTexture(GL_TEXTURE_2D, reflectionTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTex, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer Reflection is not complete!");
	}
}

void ComponentPlanarReflection::UpdateReflection(Frustum* cameraFrustum)
{
	// mirror position
	float3 mirrorPos = cameraFrustum->Pos() - planeNormal * (cameraFrustum->Pos() - GetPosition()).Dot(planeNormal) * 2.0f;
	// mirror front
	float3 mirrorFront = cameraFrustum->Front() - planeNormal * (planeNormal.Dot(cameraFrustum->Front()) * 2.0f);
	// mirror up
	float3 mirrorUp = cameraFrustum->Up() - planeNormal * (planeNormal.Dot(cameraFrustum->Up()) * 2.0f);

	Frustum frustum;
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetPerspective(cameraFrustum->HorizontalFov(), cameraFrustum->VerticalFov());
	frustum.SetViewPlaneDistances(cameraFrustum->NearPlaneDistance(), cameraFrustum->FarPlaneDistance());
	frustum.SetPos(mirrorPos);
	frustum.SetFront(mirrorFront);
	frustum.SetUp(mirrorUp);
}

void ComponentPlanarReflection::ScaleInfluenceAABB(float3& scaling)
{
	float3 center = influenceAABB.CenterPoint();

	influenceAABB.minPoint = center - scaling.Mul(originScaling);
	influenceAABB.maxPoint = center + scaling.Mul(originScaling);
}

void ComponentPlanarReflection::InternalSave(Json& meta)
{
	meta["lightType"] = GetNameByLightType(lightType).c_str();

	meta["influence_min_x"] = static_cast<float>(influenceAABB.minPoint.x);
	meta["influence_min_y"] = static_cast<float>(influenceAABB.minPoint.y);
	meta["influence_min_z"] = static_cast<float>(influenceAABB.minPoint.z);

	meta["influence_max_x"] = static_cast<float>(influenceAABB.maxPoint.x);
	meta["influence_max_y"] = static_cast<float>(influenceAABB.maxPoint.y);
	meta["influence_max_z"] = static_cast<float>(influenceAABB.maxPoint.z);

	meta["planeNormal_x"] = static_cast<float>(planeNormal.x);
	meta["planeNormal_y"] = static_cast<float>(planeNormal.y);
	meta["planeNormal_z"] = static_cast<float>(planeNormal.z);
}

void ComponentPlanarReflection::InternalLoad(const Json& meta)
{
	lightType = GetLightTypeByName(meta["lightType"]);

	influenceAABB.minPoint.x = static_cast<float>(meta["influence_min_x"]);
	influenceAABB.minPoint.y = static_cast<float>(meta["influence_min_y"]);
	influenceAABB.minPoint.z = static_cast<float>(meta["influence_min_z"]);

	influenceAABB.maxPoint.x = static_cast<float>(meta["influence_max_x"]);
	influenceAABB.maxPoint.y = static_cast<float>(meta["influence_max_y"]);
	influenceAABB.maxPoint.z = static_cast<float>(meta["influence_max_z"]);

	planeNormal.x = static_cast<float>(meta["planeNormal_x"]);
	planeNormal.y = static_cast<float>(meta["planeNormal_y"]);
	planeNormal.z = static_cast<float>(meta["planeNormal_z"]);
}

const float3 ComponentPlanarReflection::GetPosition()
{
	return GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalPosition();
}

const Quat& ComponentPlanarReflection::GetRotation()
{
	return GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalRotation();
}
