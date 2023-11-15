#include "StdAfx.h"
#include "ComponentPlanarReflection.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"

#include "Auxiliar/Utils/UtilBlur.h"

#include "Camera/Camera.h"
#include "Scene/Scene.h"

#include "ComponentTransform.h"

#include "FileSystem/Json.h"

#include "debugdraw.h"

ComponentPlanarReflection::ComponentPlanarReflection(GameObject* parent) : 
	ComponentLight(LightType::PLANAR_REFLECTION, parent, true),	frameBuffer(0), depth(0), reflectionTex(0), handle(0),
	planeNormal(float3::unitY), numMipMaps(7), distortionAmount(0.f), originScaling({ 5.f, 0.01f, 5.f }), scale(float3::one)
{
	if (GetOwner()->HasComponent<ComponentTransform>())
	{
		influenceAABB = { GetPosition() + float3(-5.f, -0.01f, -5.f), GetPosition() + float3(5.f, 0.01f, 5.f) };
		ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();

		float3 translation = float3(-0.5f);

		transform->TranslateLocalAABB(translation);
		float3 scaling = scale;
		scaling.x *= 10;
		scaling.y *= 0.05f;
		scaling.z *= 10;
		transform->ScaleLocalAABB(scaling);
	}

	frustum = new Frustum();
	frustum->SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum->SetViewPlaneDistances(0.1f, 100.0f);

	for (int mipMap = 0; mipMap < numMipMaps; mipMap++)
	{
		utilsBlur.push_back(new UtilBlur());
	}

	App->ScheduleTask([this]()
		{
			InitBuffer();
		});
}

ComponentPlanarReflection::ComponentPlanarReflection(const ComponentPlanarReflection& copyComponent) :
	ComponentLight(copyComponent), frameBuffer(0), depth(0), reflectionTex(0), handle(0), influenceAABB(copyComponent.influenceAABB),
	planeNormal(copyComponent.planeNormal), numMipMaps(copyComponent.numMipMaps), distortionAmount(copyComponent.distortionAmount), 
	originScaling(copyComponent.originScaling), scale(copyComponent.scale)
{
	frustum = new Frustum();
	frustum->SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum->SetViewPlaneDistances(0.1f, 100.0f);

	for (int mipMap = 0; mipMap < numMipMaps; mipMap++)
	{
		utilsBlur.push_back(new UtilBlur());
	}

	InitBuffer();
}

ComponentPlanarReflection::~ComponentPlanarReflection()
{
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteRenderbuffers(1, &depth);

	glDeleteTextures(1, &reflectionTex);
	glMakeTextureHandleNonResidentARB(handle);

	delete frustum;

	deleting = true;

	for (auto utilBlur : utilsBlur)
	{
		delete utilBlur;
	}

	App->GetModule<ModuleScene>()->GetLoadedScene()->RemovePlanarReflection(this);
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
	dd::arrow(GetPosition(), GetPosition() + planeNormal * 4, dd::colors::OrangeRed, 1.f);

#ifdef ENGINE
	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();

	if (transform->IsDrawBoundingBoxes())
	{
		App->GetModule<ModuleDebugDraw>()->DrawBoundingBox(transform->GetObjectOBB());
	}
#endif
}

void ComponentPlanarReflection::OnTransformChanged()
{
	// Normal rotation
	planeNormal = GetRotation() * float3(0, 1, 0);

	// Pos translation
	float3 halfsize = influenceAABB.HalfSize();

	influenceAABB.minPoint = (GetPosition() - halfsize);
	influenceAABB.maxPoint = (GetPosition() + halfsize);

	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();

	float3 translation = float3(-0.5f);

	transform->TranslateLocalAABB(translation);
}

void ComponentPlanarReflection::InitBuffer()
{
	if (frameBuffer == 0)
	{
		glGenFramebuffers(1, &frameBuffer);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	std::pair<int, int> windowSize = App->GetModule<ModuleWindow>()->GetWindowSize();

	if (depth == 0)
	{
		glGenRenderbuffers(1, &depth);
		glBindRenderbuffer(GL_RENDERBUFFER, depth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowSize.first, windowSize.second);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depth);
	}
	
	if (reflectionTex == 0)
	{
		glGenTextures(1, &reflectionTex);
		glBindTexture(GL_TEXTURE_2D, reflectionTex);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowSize.first, windowSize.second, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, numMipMaps);
		glGenerateMipmap(GL_TEXTURE_2D);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, reflectionTex, 0);
	}
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer Planar Reflection is not complete!");
	}
}

void ComponentPlanarReflection::Update()
{
	ModuleCamera* camera = App->GetModule<ModuleCamera>();
	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();

	if (IsEnabled() && camera->GetSelectedCamera()->IsInside(transform->GetEncapsuledAABB()))
	{
		UpdateReflection();
	}
}

void ComponentPlanarReflection::UpdateReflection()
{
	Frustum* cameraFrustum = App->GetModule<ModuleCamera>()->GetCamera()->GetFrustum();

	// mirror position
	float3 mirrorPos = cameraFrustum->Pos() - planeNormal * (cameraFrustum->Pos() - GetPosition()).Dot(planeNormal) * 2.0f;
	// mirror front
	float3 mirrorFront = cameraFrustum->Front() - planeNormal * (planeNormal.Dot(cameraFrustum->Front()) * 2.0f);
	mirrorFront.Normalize();
	// mirror up
	float3 mirrorUp = cameraFrustum->Up() - planeNormal * (planeNormal.Dot(cameraFrustum->Up()) * 2.0f);
	mirrorUp.Normalize();

	frustum->SetPerspective(cameraFrustum->HorizontalFov(), cameraFrustum->VerticalFov());
	frustum->SetPos(mirrorPos);
	frustum->SetFront(mirrorFront);
	frustum->SetUp(mirrorUp);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float color[4] = { 0.f, 0.f, 0.f, 0.f };
	glClearBufferfv(GL_COLOR, 0, color);

	Scene* scene = App->GetModule<ModuleScene>()->GetLoadedScene();
	ModuleRender* modRender = App->GetModule<ModuleRender>();
	ModuleProgram* modProgram = App->GetModule<ModuleProgram>();

	std::vector<GameObject*> objectsInFrustum = scene->ObtainDynamicObjectsInFrustum(frustum, NON_REFLECTIVE);

	modRender->BindCameraToProgram(modProgram->GetProgram(ProgramType::DEFAULT), *frustum);
	modRender->BindCameraToProgram(modProgram->GetProgram(ProgramType::SPECULAR), *frustum);

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(std::strlen("Planar Reflection")), "Planar Reflection");

	modRender->SortOpaques(objectsInFrustum, frustum->Pos());
	modRender->DrawMeshesByFilter(objectsInFrustum, ProgramType::DEFAULT, false);
	modRender->DrawMeshesByFilter(objectsInFrustum, ProgramType::SPECULAR, false);
	modRender->SortTransparents(objectsInFrustum, frustum->Pos());
	modRender->DrawMeshesByFilter(objectsInFrustum, ProgramType::DEFAULT);
	modRender->DrawMeshesByFilter(objectsInFrustum, ProgramType::SPECULAR);

	glPopDebugGroup();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	BlurReflection();

	App->GetModule<ModuleScene>()->GetLoadedScene()->UpdateScenePlanarReflection(this);
	App->GetModule<ModuleScene>()->GetLoadedScene()->RenderPlanarReflection(this);
}

void ComponentPlanarReflection::ScaleInfluenceAABB(float3& scaling)
{
	scale = scaling;
	float3 center = influenceAABB.CenterPoint();

	float3 valueScaled = center - scaling.Mul(originScaling);
	influenceAABB.minPoint = valueScaled;
	
	valueScaled = center + scaling.Mul(originScaling);
	influenceAABB.maxPoint = valueScaled;

	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();

	float3 translation = float3(-0.5f);

	transform->TranslateLocalAABB(translation);
	scaling.x *= 10;
	scaling.y *= 0.05f;
	scaling.z *= 10;
	transform->ScaleLocalAABB(scaling);
}

const uint64_t& ComponentPlanarReflection::GetHandleReflection()
{
	if (reflectionTex == 0)
	{
		InitBuffer();
	}
	if (handle == 0)
	{
		handle = glGetTextureHandleARB(reflectionTex);
		glMakeTextureHandleResidentARB(handle);
	}
	return handle;
}

void ComponentPlanarReflection::InternalSave(Json& meta)
{
	meta["lightType"] = GetNameByLightType(lightType).c_str();

	meta["influence_min_x"] = influenceAABB.minPoint.x;
	meta["influence_min_y"] = influenceAABB.minPoint.y;
	meta["influence_min_z"] = influenceAABB.minPoint.z;

	meta["influence_max_x"] = influenceAABB.maxPoint.x;
	meta["influence_max_y"] = influenceAABB.maxPoint.y;
	meta["influence_max_z"] = influenceAABB.maxPoint.z;

	meta["scale_x"] = scale.x;
	meta["scale_y"] = scale.y;
	meta["scale_z"] = scale.z;

	meta["planeNormal_x"] = planeNormal.x;
	meta["planeNormal_y"] = planeNormal.y;
	meta["planeNormal_z"] = planeNormal.z;
	
	meta["distortion_amount"] = distortionAmount;
}

void ComponentPlanarReflection::InternalLoad(const Json& meta)
{
	lightType = GetLightTypeByName(meta["lightType"]);

	influenceAABB.minPoint.x = static_cast<float>(meta["influence_min_x"]);
	influenceAABB.minPoint.y = static_cast<float>(meta["influence_min_y"]);
	influenceAABB.minPoint.z = static_cast<float>(meta["influence_min_z"]);

	scale.x = static_cast<float>(meta["scale_x"]);
	scale.y = static_cast<float>(meta["scale_y"]);
	scale.z = static_cast<float>(meta["scale_z"]);

	influenceAABB.maxPoint.x = static_cast<float>(meta["influence_max_x"]);
	influenceAABB.maxPoint.y = static_cast<float>(meta["influence_max_y"]);
	influenceAABB.maxPoint.z = static_cast<float>(meta["influence_max_z"]);

	planeNormal.x = static_cast<float>(meta["planeNormal_x"]);
	planeNormal.y = static_cast<float>(meta["planeNormal_y"]);
	planeNormal.z = static_cast<float>(meta["planeNormal_z"]);

	distortionAmount = static_cast<float>(meta["distortion_amount"]);

	ComponentTransform* transform = GetOwner()->GetComponentInternal<ComponentTransform>();

	float3 scaling = scale;

	float3 translation = float3(-0.5f);

	transform->TranslateLocalAABB(translation);
	scaling.x *= 10;
	scaling.y *= 0.05f;
	scaling.z *= 10;
	transform->ScaleLocalAABB(scaling);
}

void ComponentPlanarReflection::SignalEnable()
{
	ModuleScene* moduleScene = App->GetModule<ModuleScene>();

	if (moduleScene->IsLoading())
	{
		return;
	}

	Scene* currentScene = moduleScene->GetLoadedScene();

	currentScene->UpdateScenePlanarReflections();
	currentScene->RenderPlanarReflections();
}

void ComponentPlanarReflection::SignalDisable()
{
	ModuleScene* moduleScene = App->GetModule<ModuleScene>();

	if (moduleScene->IsLoading())
	{
		return;
	}

	Scene* currentScene = moduleScene->GetLoadedScene();
	currentScene->UpdateScenePlanarReflections();
	currentScene->RenderPlanarReflections();
}

void ComponentPlanarReflection::BlurReflection()
{
	ModuleWindow* window = App->GetModule<ModuleWindow>();
	int w, h;
	SDL_GetWindowSize(window->GetWindow(), &w, &h);
	unsigned int inMipResolutionW = w;
	unsigned int inMipResolutionH = h;

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(std::strlen("Planar Reflection - Gaussian Blur")),
		"Planar Reflection - Gaussian Blur");

	for (int mipMap = 0; mipMap < numMipMaps; ++mipMap)
	{
		unsigned int outMipResolutionW = static_cast<unsigned int>(w * std::pow(0.5, mipMap + 1));
		unsigned int outMipResolutionH = static_cast<unsigned int>(h * std::pow(0.5, mipMap + 1));

		utilsBlur[mipMap]->BlurTexture(reflectionTex, reflectionTex, GL_RGBA16F, GL_RGBA, GL_FLOAT, mipMap, inMipResolutionW, 
			inMipResolutionH, mipMap + 1, outMipResolutionW, outMipResolutionH);

		inMipResolutionW = outMipResolutionW;
		inMipResolutionH = outMipResolutionH;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopDebugGroup();
	glViewport(0, 0, w, h);
}

const float3& ComponentPlanarReflection::GetPosition() const
{
	return GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalPosition();
}

const Quat& ComponentPlanarReflection::GetRotation() const
{
	return GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalRotation();
}
