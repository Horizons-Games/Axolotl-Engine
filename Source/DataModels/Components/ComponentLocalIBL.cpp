#include "StdAfx.h"

#include "ComponentLocalIBL.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRender.h"
#include "ModuleProgram.h"

#include "Scene/Scene.h"

#include "ComponentTransform.h"

#include "FileSystem/Json.h"

#include "DataModels/Program/Program.h"

#include "Geometry/Frustum.h"

#include "debugdraw.h"

ComponentLocalIBL::ComponentLocalIBL(GameObject* parent) :
	ComponentLight(LightType::LOCAL_IBL, parent, true), preFiltered(0), handleIrradiance(0), handlePreFiltered(0),
	parallaxAABB({ { -10, -10, -10 }, { 10, 10, 10 } }), influenceAABB({ { -10, -10, -10 }, { 10, 10, 10 } }), 
	originCenterParallax(parallaxAABB.CenterPoint()), originCenterInfluence(influenceAABB.CenterPoint())
{
	// Generate framebuffer & renderBuffer
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	// irradianceMap
	glGenTextures(1, &diffuse);

	glBindTexture(GL_TEXTURE_CUBE_MAP, diffuse);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0,
			GL_RGB16F,
			RESOLUTION,
			RESOLUTION,
			0,
			GL_RGB,
			GL_FLOAT,
			nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// pre-filtered map
	glGenTextures(1, &preFiltered);

	glBindTexture(GL_TEXTURE_CUBE_MAP, preFiltered);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0,
			GL_RGB16F,
			RESOLUTION, //TODO look resolution
			RESOLUTION,
			0,
			GL_RGB,
			GL_FLOAT,
			nullptr);
	}

	numMipMaps = static_cast<int>(log(static_cast<float>(RESOLUTION)) / log(2));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, numMipMaps);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

ComponentLocalIBL::~ComponentLocalIBL()
{
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteTextures(1, &diffuse);
	glDeleteTextures(1, &preFiltered);

	deleting = true;

	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	if (currentScene)
	{
		currentScene->RemoveComponentLocalIBL(this);
		currentScene->UpdateSceneLocalIBLs();
		currentScene->RenderLocalIBLs();
	}
}

void ComponentLocalIBL::Update()
{
	ComponentTransform* trans = GetOwner()->GetComponentInternal<ComponentTransform>();
	Frustum frustum;
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetPerspective(math::pi / 2.0f, math::pi / 2.0f);
	frustum.SetPos(GetPosition());
	frustum.SetViewPlaneDistances(0.1f, 100.0f);

	ModuleProgram* modProgram = App->GetModule<ModuleProgram>();

	BindCameraToProgram(modProgram->GetProgram(ProgramType::G_METALLIC), frustum);
	BindCameraToProgram(modProgram->GetProgram(ProgramType::G_SPECULAR), frustum);
	BindCameraToProgram(modProgram->GetProgram(ProgramType::DEFAULT), frustum);
	BindCameraToProgram(modProgram->GetProgram(ProgramType::SPECULAR), frustum);

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(std::strlen("Light Probe")), "Light Probe");
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	// Render in Irradiance
	RenderToCubeMap(diffuse, frustum);

	// Render in pre-filtered
	for (int mipMap = 0; mipMap <= numMipMaps; ++mipMap)
	{
		float roughness = static_cast<float>(mipMap) / static_cast<float>(numMipMaps);

		unsigned int mipResolution = static_cast<unsigned int>(RESOLUTION * std::pow(0.5, mipMap));
		RenderToCubeMap(preFiltered, frustum, mipResolution, mipMap);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glPopDebugGroup();
}

void ComponentLocalIBL::Draw() const
{
	bool canDrawLocalIBL =
#ifdef ENGINE
		IsEnabled() && GetOwner() == App->GetModule<ModuleScene>()->GetSelectedGameObject();
#else
		false;
#endif // ENGINE

	if (!canDrawLocalIBL)
	{
		return;
	}

	dd::aabb(parallaxAABB.minPoint, parallaxAABB.maxPoint, dd::colors::Red);
	
	dd::aabb(influenceAABB.minPoint, influenceAABB.maxPoint, dd::colors::Green);
}

void ComponentLocalIBL::OnTransformChanged()
{
	Scene* currentScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	float3 positionParent = GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalPosition();
	float3 halfsize = parallaxAABB.HalfSize();

	parallaxAABB.minPoint = (originCenterParallax - halfsize) + positionParent;
	parallaxAABB.maxPoint = (originCenterParallax + halfsize) + positionParent;
	
	halfsize = influenceAABB.HalfSize();

	influenceAABB.minPoint = (originCenterInfluence - halfsize) + positionParent;
	influenceAABB.maxPoint = (originCenterInfluence + halfsize) + positionParent;

	currentScene->UpdateSceneLocalIBL(this);
	currentScene->RenderLocalIBL(this);
}

const uint64_t& ComponentLocalIBL::GetHandleIrradiance()
{
	if (handleIrradiance == 0)
	{
		handleIrradiance = glGetTextureHandleARB(diffuse);
		glMakeTextureHandleResidentARB(handleIrradiance);
	}
	return handleIrradiance;
}

const uint64_t& ComponentLocalIBL::GetHandlePreFiltered()
{
	if (handlePreFiltered == 0)
	{
		handlePreFiltered = glGetTextureHandleARB(preFiltered);
		glMakeTextureHandleResidentARB(handlePreFiltered);
	}
	return handlePreFiltered;
}

const float3 ComponentLocalIBL::GetPosition()
{
	return parallaxAABB.CenterPoint();
}

const Quat& ComponentLocalIBL::GetRotation()
{
	return GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalRotation();
}

const float4x4 ComponentLocalIBL::GetTransform()
{
	return float4x4(GetRotation(), GetPosition());
}

const AABB& ComponentLocalIBL::GetParallaxAABB()
{
	return parallaxAABB;
}

void ComponentLocalIBL::SetParallaxAABB(AABB& aabb)
{
	parallaxAABB = aabb;
}

const AABB& ComponentLocalIBL::GetInfluenceAABB()
{
	return influenceAABB;
}

void ComponentLocalIBL::SetInfluenceAABB(AABB& aabb)
{
	influenceAABB = aabb;
}

void ComponentLocalIBL::InternalSave(Json& meta)
{
}

void ComponentLocalIBL::InternalLoad(const Json& meta)
{
}

void ComponentLocalIBL::BindCameraToProgram(Program* program, Frustum& frustum)
{
	program->Activate();

	float3 viewPos = float3(frustum.Pos());

	program->BindUniformFloat3("viewPos", viewPos);

	program->Deactivate();
}

void ComponentLocalIBL::RenderToCubeMap(unsigned int cubemapTex, Frustum& frustum, int resolution, int mipmapLevel)
{
	const float3 front[6] = { float3::unitX,  -float3::unitX, float3::unitY,
					  -float3::unitY, float3::unitZ,  -float3::unitZ };
	const float3 up[6] = {
		-float3::unitY, -float3::unitY, float3::unitZ, -float3::unitZ, -float3::unitY, -float3::unitY
	};

	ModuleRender* modRender = App->GetModule<ModuleRender>();
	GLuint uboCamera = modRender->GetUboCamera();

	glViewport(0, 0, resolution, resolution);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, cubemapTex, mipmapLevel);
		frustum.SetFront(GetRotation() * front[i]);
		frustum.SetUp(GetRotation() * up[i]);

		glBindBuffer(GL_UNIFORM_BUFFER, uboCamera);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float4) * 4, frustum.ProjectionMatrix().ptr());
		glBufferSubData(GL_UNIFORM_BUFFER, 64, sizeof(float4) * 4, frustum.ViewMatrix().ptr());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		std::vector<GameObject*> objectsInFrustum =
			App->GetModule<ModuleScene>()->GetLoadedScene()->ObtainStaticObjectsInFrustum(&frustum);

		modRender->DrawMeshes(objectsInFrustum, float3(frustum.Pos()));
	}
}
