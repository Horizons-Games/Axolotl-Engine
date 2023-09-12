#pragma once

#include "GL/glew.h"

#define GAUSSIAN_BLUR_SHADOW_MAP 2

#define FRUSTUM_PARTITIONS 2

class Camera;
class GBuffer;
class Program;

class Shadows
{
public:
	Shadows();
	~Shadows();

	void CleanUp();
	void InitBuffers();
	void UpdateBuffers(unsigned width, unsigned height);

	void BindShadowMaps(Program* program);

	float2 ParallelReduction(GBuffer* gBuffer);
	void RenderShadowMap(const GameObject* light, const float2& minMax, Camera* camera);
	void ShadowDepthVariance();
	void GaussianBlur();

	bool UseShadows() const;
	bool UseVSM() const;

	void ToggleShadows();
	void ToggleVSM();
	void ToggleCSMDebug();

private:
	void LogarithmicPartition(Frustum* frustum);
	Frustum& ComputeLightFrustum(const GameObject* light, Frustum* cameraFrustum);

private:
	struct LightSpaceMatrices
	{
		float4x4 proj[FRUSTUM_PARTITIONS + 1];
		float4x4 view[FRUSTUM_PARTITIONS + 1];
	};

	struct CascadePlaneDistances
	{
		float4 farDistances[FRUSTUM_PARTITIONS + 1];
	};

	// Shadow Mapping buffers and textures
	GLuint shadowMapBuffer;
	GLuint gShadowMaps;
	GLuint parallelReductionInFrameBuffer;
	GLuint parallelReductionInTexture;
	GLuint parallelReductionOutFrameBuffer;
	GLuint parallelReductionOutTexture;
	GLuint minMaxBuffer;

	// Variance Shadow Mapping buffers and textures
	GLuint shadowVarianceFrameBuffer;
	GLuint shadowVarianceTexture;
	GLuint blurShadowMapBuffer[GAUSSIAN_BLUR_SHADOW_MAP];
	GLuint gBluredShadowMaps[GAUSSIAN_BLUR_SHADOW_MAP];

	GLuint uboFrustums;
	GLuint uboCascadeDistances;

	float4x4 cameraView;

	Frustum* frustums[FRUSTUM_PARTITIONS + 1];
	LightSpaceMatrices frustumMatrices;
	CascadePlaneDistances cascadeDistances;

	bool useShadows;
	bool useVarianceShadowMapping;
	bool useCSMDebug;

	std::pair<unsigned, unsigned> screenSize;
};

inline bool Shadows::UseShadows() const
{
	return useShadows;
}

inline bool Shadows::UseVSM() const
{
	return useVarianceShadowMapping;
}

inline void Shadows::ToggleShadows()
{
	useShadows = !useShadows;
}

inline void Shadows::ToggleVSM()
{
	useVarianceShadowMapping = !useVarianceShadowMapping;
}

inline void Shadows::ToggleCSMDebug()
{
	useCSMDebug = !useCSMDebug;
}
