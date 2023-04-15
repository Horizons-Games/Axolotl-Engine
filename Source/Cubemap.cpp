#include "Cubemap.h"

#include "Application.h"
#include "ModuleProgram.h"

#include "DataModels/Program/Program.h"

#include "Geometry/Frustum.h"

Cubemap::Cubemap()
{
	const float3 front[6] = { float3::unitX, -float3::unitX, float3::unitY,
								-float3::unitY, float3::unitZ, -float3::unitZ };
	float3 up[6] = { -float3::unitY, -float3::unitY, float3::unitZ,
						-float3::unitZ, -float3::unitY, -float3::unitY };
	Frustum frustum;
	frustum.SetPerspective(math::pi / 2.0f, math::pi / 2.0f);
	frustum.SetPos(float3::zero);
	frustum.SetViewPlaneDistances(0.1f, 100.0f);

	// TODO: Create and Bind Frame Buffer and Create Irradiance Cubemap
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glGenRenderbuffers(1, &renderBuffer);

	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderBuffer);
	glGenTextures(1, &enviroment);

	glBindTexture(GL_TEXTURE_CUBE_MAP, enviroment);
	for (unsigned int i = 0; i < 6; ++i)
	{
		// note that we store each face with 16 bit floating point values
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
			512, 512, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/*std::shared_ptr<ResourceSkyBox> resourceSkybox =
		App->resources->RequestResource<ResourceSkyBox>("Assets/Cubemaps/SunsetSkyboxHDR.hdr");*/

	Program* irradianceProgram = App->program->GetProgram(ProgramType::IRRADIANCE_MAP);
	irradianceProgram->Activate();
	float4x4 projMatrix = frustum.ProjectionMatrix();
	irradianceProgram->BindUniformFloat4x4("proj", (const float*)&projMatrix, GL_TRUE);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			enviroment, 0);
		frustum.SetFront(front[i]);
		frustum.SetUp(up[i]);
		// TODO: Draw Unit Cube using frustum view and projection matrices
		float4x4 viewMatrix = frustum.ViewMatrix();
		irradianceProgram->BindUniformFloat4x4("proj", (const float*)&viewMatrix, GL_TRUE);
	}

	irradianceProgram->Deactivate();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

Cubemap::~Cubemap()
{
	glDeleteFramebuffers(1, &frameBuffer);
	glDeleteRenderbuffers(1, &renderBuffer);
	glDeleteTextures(1, &enviroment);
}
