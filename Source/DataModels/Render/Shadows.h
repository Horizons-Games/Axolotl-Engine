#pragma once

#include "GL/glew.h"

#define GAUSSIAN_BLUR_SHADOW_MAP 2
#define FRUSTUM_PARTITIONS 1
#define FRUSTUM_INTERVALS { 0.25 }

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

	void BindShadowMap(Program* program);

	float2 ParallelReduction(GBuffer* gBuffer);
	void RenderShadowMap(const GameObject* light, const float2& minMax, Camera* camera);
	void ShadowDepthVariance();
	void GaussianBlur();

	bool UseShadows() const;
	bool UseVSM() const;

	void ToggleShadows();
	void ToggleVSM();

private:
	// Shadow Mapping buffers and textures
	GLuint shadowMapBuffer = 0;
	GLuint gShadowMap = 0;
	GLuint parallelReductionInTexture = 0;
	GLuint parallelReductionOutTexture = 0;
	GLuint minMaxBuffer = 0;

	// Variance Shadow Mapping buffers and textures
	GLuint shadowVarianceTexture = 0;
	GLuint blurShadowMapBuffer[GAUSSIAN_BLUR_SHADOW_MAP];
	GLuint gBluredShadowMap[GAUSSIAN_BLUR_SHADOW_MAP];

	float4x4 lightView;
	float4x4 lightProj;

	bool useShadows;
	bool useVarianceShadowMapping;

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
