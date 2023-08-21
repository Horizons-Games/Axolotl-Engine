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

ComponentLocalIBL::ComponentLocalIBL(bool active, GameObject* owner) :
	Component(ComponentType::LOCAL_IBL, active, owner, true), preFiltered(0), handleIrradiance(0), handlePreFiltered(0),
	aabb({ { 0, 0, 0 }, { 1, 1, 1 } })
{
	// Generate framebuffer & renderBuffer
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	// irradianceMap
	glGenTextures(1, &irradiance);

	glBindTexture(GL_TEXTURE_CUBE_MAP, irradiance);
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
	glDeleteTextures(1, &irradiance);
	glDeleteTextures(1, &preFiltered);
}

void ComponentLocalIBL::Update()
{
	ComponentTransform* trans = GetOwner()->GetComponentInternal<ComponentTransform>();
	Frustum frustum;
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetPerspective(math::pi / 2.0f, math::pi / 2.0f);
	frustum.SetPos(trans->GetGlobalPosition());
	frustum.SetViewPlaneDistances(0.1f, 100.0f);

	ModuleProgram* modProgram = App->GetModule<ModuleProgram>();

	BindCameraToProgram(modProgram->GetProgram(ProgramType::G_METALLIC), frustum);
	BindCameraToProgram(modProgram->GetProgram(ProgramType::G_SPECULAR), frustum);
	BindCameraToProgram(modProgram->GetProgram(ProgramType::DEFAULT), frustum);
	BindCameraToProgram(modProgram->GetProgram(ProgramType::SPECULAR), frustum);

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(std::strlen("Light Probe")), "Light Probe");
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	// Render in Irradiance
	RenderToCubeMap(irradiance, frustum);

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

	dd::aabb(aabb.minPoint, aabb.maxPoint, dd::colors::Red);
}

const uint64_t& ComponentLocalIBL::GetHandleIrradiance()
{
	if (handleIrradiance == 0)
	{
		handleIrradiance = glGetTextureHandleARB(irradiance);
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

const float3& ComponentLocalIBL::GetPosition()
{
	return GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalPosition();
}

const Quat& ComponentLocalIBL::GetRotation()
{
	return GetOwner()->GetComponentInternal<ComponentTransform>()->GetGlobalRotation();
}

const float4x4& ComponentLocalIBL::GetTransform()
{
	return float4x4(GetRotation(), GetPosition());
}

const AABB& ComponentLocalIBL::GetAABB()
{
	return aabb;
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
		frustum.SetFront(front[i]);
		frustum.SetUp(up[i]);

		glBindBuffer(GL_UNIFORM_BUFFER, uboCamera);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float4) * 4, frustum.ProjectionMatrix().ptr());
		glBufferSubData(GL_UNIFORM_BUFFER, 64, sizeof(float4) * 4, frustum.ViewMatrix().ptr());
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		std::vector<GameObject*> objectsInFrustum =
			App->GetModule<ModuleScene>()->GetLoadedScene()->ObtainStaticObjectsInFrustum(&frustum);

		modRender->DrawMeshes(objectsInFrustum, float3(frustum.Pos()));
	}
}
