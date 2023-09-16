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

#define LAMBDA 0.85f

Shadows::Shadows()
{
	shadowMapBuffer = 0;
	gShadowMaps = 0;
	parallelReductionInFrameBuffer = 0;
	parallelReductionInTexture = 0;
	parallelReductionOutFrameBuffer = 0;
	parallelReductionOutTexture = 0;
	shadowVarianceFrameBuffer = 0;
	shadowVarianceTexture = 0;
	uboFrustums = 0;
	uboCascadeDistances = 0;
	ssboMinMax = 0;
	ssboLogSplit = 0;

	for (unsigned i = 0; i < GAUSSIAN_BLUR_SHADOW_MAP; ++i)
	{
		blurShadowMapBuffer[i] = 0;
		gBluredShadowMaps[i] = 0;
	}

	for (unsigned i = 0; i <= FRUSTUM_PARTITIONS; ++i)
	{
		frustums[i] = new Frustum();
	}

	useShadows = true;
	useVarianceShadowMapping = true;
	useCSMDebug = false;

	lambda = LAMBDA;
}

Shadows::~Shadows()
{
	for (unsigned i = 0; i <= FRUSTUM_PARTITIONS; ++i)
	{
		delete frustums[i];
	}

	CleanUp();
}

void Shadows::CleanUp()
{
	glDeleteFramebuffers(1, &shadowMapBuffer);
	glDeleteTextures(1, &gShadowMaps);

	glDeleteFramebuffers(GAUSSIAN_BLUR_SHADOW_MAP, &blurShadowMapBuffer[0]);
	glDeleteTextures(GAUSSIAN_BLUR_SHADOW_MAP, &gBluredShadowMaps[0]);

	glDeleteFramebuffers(1, &parallelReductionInFrameBuffer);
	glDeleteTextures(1, &parallelReductionInTexture);
	glDeleteFramebuffers(1, &parallelReductionOutFrameBuffer);
	glDeleteTextures(1, &parallelReductionOutTexture);
	glDeleteFramebuffers(1, &shadowVarianceFrameBuffer);
	glDeleteTextures(1, &shadowVarianceTexture);

	glDeleteBuffers(1, &ssboMinMax);
	glDeleteBuffers(1, &ssboLogSplit);

	glDeleteBuffers(1, &uboFrustums);
	glDeleteBuffers(1, &uboCascadeDistances);
}

void Shadows::InitBuffers()
{
	glGenFramebuffers(1, &shadowMapBuffer);
	glGenTextures(1, &gShadowMaps);

	glGenFramebuffers(GAUSSIAN_BLUR_SHADOW_MAP, &blurShadowMapBuffer[0]);
	glGenTextures(GAUSSIAN_BLUR_SHADOW_MAP, &gBluredShadowMaps[0]);

	glGenFramebuffers(1, &parallelReductionInFrameBuffer);
	glGenTextures(1, &parallelReductionInTexture);
	glGenFramebuffers(1, &parallelReductionOutFrameBuffer);
	glGenTextures(1, &parallelReductionOutTexture);
	glGenFramebuffers(1, &shadowVarianceFrameBuffer);
	glGenTextures(1, &shadowVarianceTexture);

	glGenBuffers(1, &ssboMinMax);
	glGenBuffers(1, &ssboLogSplit);

	glGenBuffers(1, &uboFrustums);
	glGenBuffers(1, &uboCascadeDistances);
}

void Shadows::UpdateBuffers(unsigned width, unsigned height)
{
	screenSize = std::make_pair(width, height);

	// Shadow Mapping buffers
	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapBuffer);

	glBindTexture(GL_TEXTURE_2D_ARRAY, gShadowMaps);
	glTexImage3D(GL_TEXTURE_2D_ARRAY, 
				 0,
				 GL_DEPTH_COMPONENT32F, 
				 screenSize.first, screenSize.second, 
				 static_cast<GLint>(FRUSTUM_PARTITIONS + 1),
				 0,
				 GL_DEPTH_COMPONENT,
				 GL_FLOAT, 
				 NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, gShadowMaps, 0);

	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("ERROR::FRAMEBUFFER:: Shadow Map framebuffer not completed!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, parallelReductionInFrameBuffer);

	glBindTexture(GL_TEXTURE_2D, parallelReductionInTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, screenSize.first, screenSize.second, 0, GL_RG, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, parallelReductionInTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer Parallel in is not complete!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, parallelReductionOutFrameBuffer);

	glBindTexture(GL_TEXTURE_2D, parallelReductionOutTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, screenSize.first, screenSize.second, 0, GL_RG, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, parallelReductionOutTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer Parallel out is not complete!");
	}

	//VSM Buffers

	glBindFramebuffer(GL_FRAMEBUFFER, shadowVarianceFrameBuffer);

	glBindTexture(GL_TEXTURE_2D_ARRAY, shadowVarianceTexture);
	glTexImage3D(GL_TEXTURE_2D_ARRAY,
		0,
		GL_RG32F,
		screenSize.first, screenSize.second,
		static_cast<GLint>(FRUSTUM_PARTITIONS + 1),
		0,
		GL_RG,
		GL_FLOAT,
		NULL);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, shadowVarianceTexture, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		LOG_ERROR("ERROR::FRAMEBUFFER:: Framebuffer is not complete!");
	}

	for (unsigned i = 0; i < GAUSSIAN_BLUR_SHADOW_MAP; ++i)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, blurShadowMapBuffer[i]);

		glBindTexture(GL_TEXTURE_2D_ARRAY, gBluredShadowMaps[i]);
		glTexImage3D(GL_TEXTURE_2D_ARRAY, 
					 0, 
					 GL_RG32F, 
					 screenSize.first, screenSize.second, 
					 static_cast<GLint>(FRUSTUM_PARTITIONS + 1),
					 0, 
					 GL_RG, 
					 GL_FLOAT, 
					 NULL);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, gBluredShadowMaps[i], 0);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		{
			LOG_ERROR("ERROR::FRAMEBUFFER:: Blured Shadow Map framebuffer (num {}) not completed!", i);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, uboFrustums);
	glBindBufferRange(GL_UNIFORM_BUFFER, 2, uboFrustums, 0, sizeof(LightSpaceMatrices));

	glBindBuffer(GL_UNIFORM_BUFFER, uboCascadeDistances);
	glBindBufferRange(GL_UNIFORM_BUFFER, 3, uboCascadeDistances, 0, sizeof(CascadePlaneDistances));

	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Shadows::BindShadowMaps(Program* program)
{
	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_2D_ARRAY, useVarianceShadowMapping ?
		gBluredShadowMaps[GAUSSIAN_BLUR_SHADOW_MAP - 1] : gShadowMaps);

	glBindBuffer(GL_UNIFORM_BUFFER, uboCascadeDistances);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(CascadePlaneDistances), &cascadeDistances, GL_STATIC_DRAW);

	program->BindUniformFloat4x4("cameraViewMatrix", reinterpret_cast<const float*>(&cameraView), GL_TRUE);
	program->BindUniformInt("toggleCSMDebug", static_cast<int>(useCSMDebug));
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

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboMinMax);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(minMax), &minMax[0], GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboMinMax);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glDispatchCompute(1, 1, 1);
	glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboMinMax);
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

	cameraFrustum->SetViewPlaneDistances(lightNear, lightFar);

	// Compute whole camera frustum to do culling only once
	math::Frustum frustum = ComputeLightFrustum(light, cameraFrustum);
	std::vector<GameObject*> objectsInFrustum =
		App->GetModule<ModuleScene>()->GetLoadedScene()->ObtainObjectsInFrustum(&frustum);

	// Calculate sub frustum
	LogarithmicPartition(cameraFrustum);

	for (int i = 0; i <= FRUSTUM_PARTITIONS; ++i)
	{
		frustum = ComputeLightFrustum(light, frustums[i]);

		frustumMatrices.proj[i] = frustum.ProjectionMatrix();
		frustumMatrices.view[i] = frustum.ViewMatrix();
	}

	cameraView = cameraFrustum->ViewMatrix();

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(std::strlen("Shadow Mapping")),
		"Shadow Mapping");

	// Program binding
	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::SHADOW_MAPPING);
	program->Activate();

	glBindFramebuffer(GL_FRAMEBUFFER, shadowMapBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);

	glBindBuffer(GL_UNIFORM_BUFFER, uboFrustums);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(LightSpaceMatrices), &frustumMatrices, GL_STATIC_DRAW);

	glCullFace(GL_BACK);

	ModuleRender* render = App->GetModule<ModuleRender>();
	render->GetBatchManager()->DrawMeshes(objectsInFrustum, float3(frustum.Pos()));

	glCullFace(GL_FRONT);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	program->Deactivate();

	glPopDebugGroup();
}

void Shadows::ShadowDepthVariance()
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, std::strlen("Shadow Depth Variance"), "Shadow Depth Variance");

	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::SHADOW_DEPTH_VARIANCE);
	program->Activate();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, gShadowMaps);

	glBindImageTexture(0, shadowVarianceTexture, 0, GL_TRUE, 0, GL_WRITE_ONLY, GL_RG32F);

	program->BindUniformInt2("inSize", screenSize.first, screenSize.second);
	program->BindUniformInt("cascadeCount", FRUSTUM_PARTITIONS + 1);

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

	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::GAUSSIAN_BLUR_3D);
	program->Activate();

	glBindFramebuffer(GL_FRAMEBUFFER, blurShadowMapBuffer[0]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, shadowVarianceTexture);

	program->BindUniformFloat2("invSize", float2(1.0f / screenSize.first, 1.0f / screenSize.second));
	program->BindUniformFloat2("blurDirection", float2(1.0f, 0.0f));

	glDrawArrays(GL_TRIANGLES, 0, 3);

	glBindFramebuffer(GL_FRAMEBUFFER, blurShadowMapBuffer[1]);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, gBluredShadowMaps[0]);

	program->BindUniformFloat2("blurDirection", float2(0.0f, 1.0f));

	glDrawArrays(GL_TRIANGLES, 0, 3);

	program->Deactivate();

	glPopDebugGroup();
}

void Shadows::LogarithmicPartition(Frustum* frustum)
{
	float nearPlane = frustum->NearPlaneDistance();
	float farPlane = frustum->FarPlaneDistance();
	float lastFarPlane = nearPlane;

	Program* program = App->GetModule<ModuleProgram>()->GetProgram(ProgramType::LOG_SPLIT);
	program->Activate();

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, static_cast<GLsizei>(std::strlen("logarithmic Split")), 
					 "logarithmic Split");

	program->BindUniformFloat2("nearFar", float2(nearPlane, farPlane));
	program->BindUniformInt("splits", FRUSTUM_PARTITIONS);
	program->BindUniformFloat("lambda", lambda);

	float splitPositions[FRUSTUM_PARTITIONS];

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboLogSplit);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(splitPositions), &splitPositions[0], GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboLogSplit);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glDispatchCompute(FRUSTUM_PARTITIONS, 1, 1);
	glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboLogSplit);
	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(splitPositions), &splitPositions[0]);

	glPopDebugGroup();

	program->Deactivate();

	for (unsigned i = 0; i < FRUSTUM_PARTITIONS; ++i)
	{
		float splitPosition = splitPositions[i];

		*frustums[i] = *frustum;
		frustums[i]->SetViewPlaneDistances(lastFarPlane, splitPosition);

		cascadeDistances.farDistances[i].x = splitPosition;

		lastFarPlane = splitPosition;
	}

	*frustums[FRUSTUM_PARTITIONS] = *frustum;
	frustums[FRUSTUM_PARTITIONS]->SetViewPlaneDistances(lastFarPlane, farPlane);

	cascadeDistances.farDistances[FRUSTUM_PARTITIONS].x = farPlane;
}

Frustum& Shadows::ComputeLightFrustum(const GameObject* light, Frustum* cameraFrustum)
{
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

	return frustum;
}
