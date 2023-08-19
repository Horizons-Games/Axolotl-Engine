#include "StdAfx.h"

#include "ComponentLightProbe.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleRender.h"
#include "ModuleProgram.h"
#include "ModuleWindow.h"

#include "Scene/Scene.h"

#include "ComponentTransform.h"

#include "FileSystem/Json.h"

#include "DataModels/Program/Program.h"

#include "Geometry/Frustum.h"

ComponentLightProbe::ComponentLightProbe(bool active, GameObject* owner) :
	Component(ComponentType::LIGHT_PROBE, active, owner, true), preFiltered(0)
{
	ModuleWindow* window = App->GetModule<ModuleWindow>();
	std::pair<int, int> windowSize = window->GetWindowSize();

	// Generate framebuffer & renderBuffer
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);

	// pre-filtered map
	glGenTextures(1, &preFiltered);

	glBindTexture(GL_TEXTURE_CUBE_MAP, preFiltered);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0,
			GL_RGB16F,
			windowSize.first, //TODO look resolution
			windowSize.second,
			0,
			GL_RGB,
			GL_FLOAT,
			nullptr);
	}

	//numMipMaps = static_cast<int>(log(static_cast<float>(PRE_FILTERED_MAP_RESOLUTION)) / log(2));
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	//glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, numMipMaps);
	//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

ComponentLightProbe::~ComponentLightProbe()
{
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteTextures(1, &preFiltered);
}

void ComponentLightProbe::Update()
{
	const float3 front[6] = { float3::unitX,  -float3::unitX, float3::unitY,
						  -float3::unitY, float3::unitZ,  -float3::unitZ };
	const float3 up[6] = {
		-float3::unitY, -float3::unitY, float3::unitZ, -float3::unitZ, -float3::unitY, -float3::unitY
	};

	ComponentTransform* trans = GetOwner()->GetComponentInternal<ComponentTransform>();

	Frustum frustum;
	frustum.SetKind(FrustumSpaceGL, FrustumRightHanded);
	frustum.SetPerspective(math::pi / 2.0f, math::pi / 2.0f);
	frustum.SetPos(trans->GetGlobalPosition());
	frustum.SetViewPlaneDistances(0.1f, 100.0f);

	ModuleRender* modRender = App->GetModule<ModuleRender>();
	ModuleProgram* modProgram = App->GetModule<ModuleProgram>();

	GLuint uboCamera = modRender->GetUboCamera();

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(std::strlen("Light Probe")), "Light Probe");
	for (unsigned int i = 0; i < 6; ++i)
	{
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, preFiltered, 0);
		frustum.SetFront(front[i]);
		frustum.SetUp(up[i]);

		std::vector<GameObject*> objectsInFrustum =
			App->GetModule<ModuleScene>()->GetLoadedScene()->ObtainObjectsInFrustum(&frustum); // TODO obtain static Only

		BindCameraToProgram(modProgram->GetProgram(ProgramType::G_METALLIC), uboCamera, frustum);
		BindCameraToProgram(modProgram->GetProgram(ProgramType::G_SPECULAR), uboCamera, frustum);
		BindCameraToProgram(modProgram->GetProgram(ProgramType::DEFAULT), uboCamera, frustum);
		BindCameraToProgram(modProgram->GetProgram(ProgramType::SPECULAR), uboCamera, frustum);

		modRender->DrawMeshes(objectsInFrustum, float3(frustum.Pos()));
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glPopDebugGroup();
}

void ComponentLightProbe::InternalSave(Json& meta)
{
}

void ComponentLightProbe::InternalLoad(const Json& meta)
{
}

void ComponentLightProbe::BindCameraToProgram(Program* program, GLuint uboCamera, Frustum frustum)
{
	program->Activate();

	float3 viewPos = float3(frustum.Pos());

	glBindBuffer(GL_UNIFORM_BUFFER, uboCamera);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float4) * 4, frustum.ProjectionMatrix().ptr());
	glBufferSubData(GL_UNIFORM_BUFFER, 64, sizeof(float4) * 4, frustum.ViewMatrix().ptr());
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	program->BindUniformFloat3("viewPos", viewPos);

	program->Deactivate();
}
