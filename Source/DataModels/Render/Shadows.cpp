#include "StdAfx.h"
#include "Shadows.h"

#include "Application.h"

#include "Batch/BatchManager.h"

#include "Camera/Camera.h"
#include "Camera/CameraGameObject.h"

#include "Components/ComponentCamera.h"
#include "Components/ComponentTransform.h"

#include "GameObject/GameObject.h"

#include "GBuffer/GBuffer.h"

#include "Geometry/Frustum.h"

#include "Modules/ModuleCamera.h"
#include "Modules/ModuleProgram.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleScene.h"

#include "Program/Program.h";

#include "Scene/Scene.h"

#include "debugdraw.h"

Shadows::Shadows()
{
	shadowMapBuffer = 0;
	gShadowMap = 0;
	parallelReductionInTexture = 0;
	parallelReductionOutTexture = 0;
	minMaxBuffer = 0;

	for (unsigned i = 0; i < GAUSSIAN_BLUR_SHADOW_MAP; ++i)
	{
		blurShadowMapBuffer[i] = 0;
		gBluredShadowMap[i] = 0;
	}

	useShadows = true;
	useVarianceShadowMapping = true;
}

Shadows::~Shadows()
{
	CleanUp();
}

void Shadows::CleanUp()
{
	glDeleteFramebuffers(1, &shadowMapBuffer);
	glDeleteTextures(1, &gShadowMap);

	glDeleteFramebuffers(GAUSSIAN_BLUR_SHADOW_MAP, &blurShadowMapBuffer[0]);
	glDeleteTextures(GAUSSIAN_BLUR_SHADOW_MAP, &gBluredShadowMap[0]);

	glDeleteTextures(1, &parallelReductionInTexture);
	glDeleteTextures(1, &parallelReductionOutTexture);
	glDeleteTextures(1, &shadowVarianceTexture);

	glDeleteBuffers(1, &minMaxBuffer);
}

void Shadows::InitBuffers()
{
	glGenFramebuffers(1, &shadowMapBuffer);
	glGenTextures(1, &gShadowMap);

	glGenFramebuffers(GAUSSIAN_BLUR_SHADOW_MAP, &blurShadowMapBuffer[0]);
	glGenTextures(GAUSSIAN_BLUR_SHADOW_MAP, &gBluredShadowMap[0]);

	glGenTextures(1, &parallelReductionInTexture);
	glGenTextures(1, &parallelReductionOutTexture);
	glGenTextures(1, &shadowVarianceTexture);

	glGenBuffers(1, &minMaxBuffer);
}

void Shadows::UpdateBuffers(unsigned width, unsigned height)
{
	screenSize = std::make_pair(width, height);

	// Shadow Mapping buffers
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapBuffer);

	glBindTexture(GL_TEXTURE_2D, gShadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, screenSize.first, screenSize.second, 0, GL_DEPTH_COMPONENT,
				 GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, gShadowMap, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("ERROR::FRAMEBUFFER:: Shadow Map framebuffer not completed!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindTexture(GL_TEXTURE_2D, parallelReductionInTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, screenSize.first, screenSize.second);
	glBindTexture(GL_TEXTURE_2D, parallelReductionOutTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, screenSize.first, screenSize.second);

	//VSM Buffers

	glBindTexture(GL_TEXTURE_2D, shadowVarianceTexture);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RG32F, screenSize.first, screenSize.second);

	for (unsigned i = 0; i < GAUSSIAN_BLUR_SHADOW_MAP; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, blurShadowMapBuffer[i]);

		glBindTexture(GL_TEXTURE_2D, gBluredShadowMap[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, screenSize.first, screenSize.second, 0, GL_RG, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gBluredShadowMap[i], 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			LOG_ERROR("ERROR::FRAMEBUFFER:: Blured Shadow Map framebuffer (num {}) not completed!", i);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Shadows::BindShadowMap(Program* program)
{
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D, useVarianceShadowMapping ? 
		gBluredShadowMap[GAUSSIAN_BLUR_SHADOW_MAP - 1] : gShadowMap);

	float4x4 lightSpaceMatrix = lightProj * lightView;

	program->BindUniformFloat4x4("lightSpaceMatrix", reinterpret_cast<const float*>(&lightSpaceMatrix), GL_TRUE);
}

float2 Shadows::ParallelReduction(GBuffer* gBuffer)
{
	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::PARALLEL_REDUCTION);
	program->Activate();

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(std::strlen("Parallel Reduction")),
		"Parallel Reduction");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBuffer->GetDepthTexture());

	//Use the texture as an image
	glBindImageTexture(0, parallelReductionInTexture, 0, false, 0, GL_WRITE_ONLY, GL_RG32F);

	program->BindUniformInt2("inSize", screenSize.first, screenSize.second);

	unsigned int numGroupsX = (screenSize.first + (16 - 1)) / 16;
	unsigned int numGroupsY = (screenSize.second + (8 - 1)) / 8;

	glDispatchCompute(numGroupsX, numGroupsY, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	GLuint srcTexture = parallelReductionInTexture;
	GLuint dstTexture = parallelReductionOutTexture;

	while (numGroupsX > 1 || numGroupsY > 1)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, srcTexture);

		//Use the texture as an image
		glBindImageTexture(0, dstTexture, 0, false, 0, GL_WRITE_ONLY, GL_RG32F);

		program->BindUniformInt2("inSize", numGroupsX, numGroupsY);

		numGroupsX = (numGroupsX + (8 - 1)) / 8;
		numGroupsY = (numGroupsY + (4 - 1)) / 4;

		glDispatchCompute(numGroupsX, numGroupsY, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		std::swap(srcTexture, dstTexture);
	}

	glPopDebugGroup();

	program->Deactivate();

	program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::MIN_MAX);
	program->Activate();

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(std::strlen("Min Max")), "Min Max");

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, srcTexture);

	float2 minMax(0.0f, 0.0f);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, minMaxBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(minMax), &minMax[0], GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, minMaxBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glDispatchCompute(1, 1, 1);
	glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, minMaxBuffer);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(minMax), &minMax[0]);

	glPopDebugGroup();

	program->Deactivate();

	//glGetTextureSubImage(srcTexture, 0, 0, 0, 0, 1, 1, 1, GL_RG, GL_FLOAT, sizeof(float2), &minMax);

	glBindTexture(GL_TEXTURE_2D, 0);

	return minMax;
}

void Shadows::RenderShadowMap(const GameObject* light, const float2& minMax, Camera* camera)
{
	math::Frustum* cameraFrustum = new Frustum(*camera->GetFrustum());
	
	// SDSM: Calculus for the final near an far planes
	float n = cameraFrustum->NearPlaneDistance();
	float f = cameraFrustum->FarPlaneDistance();
	float T = -(n + f) / (f - n);
	float S = (-2 * f * n) / (f - n);
	float lightNear = S / (T + minMax[0]);
	float lightFar = S / (T + minMax[1]);

	if (lightNear > lightFar) {
		lightNear = 0.1f;
	}

	cameraFrustum->SetViewPlaneDistances(lightNear, lightFar);

	// Compute camera frustrum bounding sphere
	float3 corners[8];
	cameraFrustum->GetCornerPoints(corners);

	const float3 sphereCenter = cameraFrustum->CenterPoint();
	float sphereRadius = 0.0f;

	for (unsigned int i = 0; i < 8; ++i)
	{
		sphereRadius = std::max(sphereRadius, sphereCenter.Distance(corners[i]));
	}

	// Compute bounding box
	math::Frustum frustum;

	const ComponentTransform* lightTransform = light->GetComponent<ComponentTransform>();
	const float3& lightPos = lightTransform->GetGlobalPosition();

	float3 lightFront = lightTransform->GetGlobalForward();
	float3 lightRight = Cross(lightFront, float3(0.0f, 1.0f, 0.0f)).Normalized();
	float3 lifghtUp = math::Cross(lightRight, lightFront).Normalized();
	frustum.SetKind(FrustumProjectiveSpace::FrustumSpaceGL, FrustumHandedness::FrustumRightHanded);
	frustum.SetPos(sphereCenter - lightFront * sphereRadius);
	frustum.SetFront(lightFront);
	frustum.SetUp(lifghtUp);
	frustum.SetViewPlaneDistances(0.0f, sphereRadius * 2.0f);
	frustum.SetOrthographic(sphereRadius * 2.0f, sphereRadius * 2.0f);

	//dd::frustum(cameraFrustum->ViewProjMatrix().Inverted(), dd::colors::Yellow);
	//dd::frustum(frustum.ViewProjMatrix().Inverted(), dd::colors::Yellow);;

	// Frustum culling with the created light frustum to obtain the meshes of the scene to take into account
	std::vector<GameObject*> objectsInFrustum =
		App->GetModule<ModuleScene>()->GetLoadedScene()->ObtainObjectsInFrustum(&frustum);

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(std::strlen("Shadow Mapping")),
		"Shadow Mapping");

	// Program binding
	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::SHADOW_MAPPING);
	program->Activate();

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	lightView = frustum.ViewMatrix();
	lightProj = frustum.ProjectionMatrix();

	ModuleRender* render = App->GetModule<ModuleRender>();

	glBindBuffer(GL_UNIFORM_BUFFER, render->GetCameraUBO());
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(float4) * 4, &lightProj[0]);
	glBufferSubData(GL_UNIFORM_BUFFER, 64, sizeof(float4) * 4, &lightView[0]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	render->GetBatchManager()->DrawMeshes(objectsInFrustum, float3(frustum.Pos()));

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	program->Deactivate();

	glPopDebugGroup();
}

void Shadows::ShadowDepthVariance()
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, std::strlen("Shadow Depth Variance"), "Shadow Depth Variance");

	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::SHADOW_DEPTH_VARIANCE);
	program->Activate();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gShadowMap);

	glBindImageTexture(0, shadowVarianceTexture, 0, false, 0, GL_WRITE_ONLY, GL_RG32F);

	program->BindUniformInt2("inSize", screenSize.first, screenSize.second);

	unsigned int numGroupsX = (screenSize.first + (16 - 1)) / 16;
	unsigned int numGroupsY = (screenSize.second + (8 - 1)) / 8;

	glDispatchCompute(numGroupsX, numGroupsY, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glBindTexture(GL_TEXTURE_2D, 0);

	program->Deactivate();

	glPopDebugGroup();
}

void Shadows::GaussianBlur()
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, std::strlen("Gaussian Blur"), "Gaussian Blur");

	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::GAUSSIAN_BLUR);
	program->Activate();

	glBindFramebuffer(GL_FRAMEBUFFER, blurShadowMapBuffer[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, shadowVarianceTexture);

	program->BindUniformFloat2("invSize", float2(1.0f / screenSize.first, 1.0f / screenSize.second));
	program->BindUniformFloat2("blurDirection", float2(1.0f, 0.0f));

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindFramebuffer(GL_FRAMEBUFFER, blurShadowMapBuffer[1]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gBluredShadowMap[0]);

	program->BindUniformFloat2("blurDirection", float2(0.0f, 1.0f));

	glDrawArrays(GL_TRIANGLES, 0, 3);

	program->Deactivate();

	glPopDebugGroup();
}
