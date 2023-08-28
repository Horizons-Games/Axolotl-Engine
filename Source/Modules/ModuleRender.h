#pragma once

#include "Module.h"

#include "GL/glew.h"
#include "Math/float4.h"

#include "FileSystem/UID.h"

#define KAWASE_DUAL_SAMPLERS 4

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class Quadtree;
class Program;
class Cubemap;
class GameObject;
class GeometryBatch;
class BatchManager;
class ComponentMeshRenderer;
class GBuffer;

class ModuleRender : public Module
{
public:

	ModuleRender();
	~ModuleRender() override;

	bool Init() override;

	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;

	bool CleanUp() override;

	void WindowResized(unsigned width, unsigned height);
	void UpdateBuffers(unsigned width, unsigned height);

	void SetBackgroundColor(float4 color);
	float4 GetBackgroundColor() const;

	void ChangeRenderMode();
	void ChangeToneMapping();
	void SwitchBloomActivation();
	void ToggleShadows();

	GLuint GetRenderedTexture() const;
	float GetObjectDistance(const GameObject* gameObject);

	BatchManager* GetBatchManager() const;

	void FillRenderList(const Quadtree* quadtree);
	void AddToRenderList(const GameObject* gameObject);

	bool IsObjectInsideFrustrum(const GameObject* gameObject);

	void DrawQuadtree(const Quadtree* quadtree);

	void FillCharactersBatches();
	void RelocateGOInBatches(GameObject* go);
	
	float2 ParallelReduction(Program* program, int width, int height);
	void RenderShadowMap(const GameObject* light, const float2& minMax);

private:

	enum class ModeRender {
		DEFAULT = 0,
		POSITION = 1,
		NORMAL = 2,
		DIFFUSE = 3,
		SPECULAR = 4,
		EMISSIVE = 5,
		LENGTH
	};

	enum class ToneMappingMode {
		NONE = 0,
		UNCHARTED2 = 1,
		ACES_FILM = 2,
		LENGTH
	};

	bool CheckIfTransparent(const GameObject* gameObject);

	void DrawHighlight(GameObject* gameObject);

	void BindCameraToProgram(Program* program);
	void BindCubemapToProgram(Program* program);

	void KawaseDualFiltering();

	void* context;

	float4 backgroundColor;

	float4x4 dirLightView;
	float4x4 dirLightProj;

	BatchManager* batchManager;
	GBuffer* gBuffer;

	unsigned uboCamera;

	unsigned modeRender;
	unsigned toneMappingMode;
	unsigned bloomActivation;
	
	std::unordered_set<const GameObject*> gameObjectsInFrustrum;
	std::unordered_map<const GameObject*, float> objectsInFrustrumDistances;

	// 0: used in game and engine 
	// 1: only in engine, stores the final result, to avoid writing and reading at the same time
	GLuint frameBuffer[2];
	GLuint renderedTexture[2];

	// Ping-pong buffers to kawase dual filtering bloom
	GLuint dualKawaseDownFramebuffers[KAWASE_DUAL_SAMPLERS];
	GLuint dualKawaseDownTextures[KAWASE_DUAL_SAMPLERS];
	GLuint dualKawaseUpFramebuffers[KAWASE_DUAL_SAMPLERS];
	GLuint dualKawaseUpTextures[KAWASE_DUAL_SAMPLERS];
	
	// Shadow Mapping buffers and textures
	GLuint depthStencilRenderBuffer;
	GLuint shadowMapBuffer;
	GLuint gShadowMap;
	GLuint parallelReductionInTexture;
	GLuint parallelReductionOutTexture;
	GLuint minMaxBuffer;

	bool renderShadows;

	friend class ModuleEditor;
};

inline void ModuleRender::SetBackgroundColor(float4 color)
{
	backgroundColor = color;
}

inline float4 ModuleRender::GetBackgroundColor() const
{
	return backgroundColor;
}

inline void ModuleRender::ChangeRenderMode()
{
	modeRender = (modeRender + 1) % static_cast<int>(ModeRender::LENGTH);
}

inline void ModuleRender::ChangeToneMapping()
{
	toneMappingMode = (toneMappingMode + 1) % static_cast<int>(ToneMappingMode::LENGTH);
}

inline void ModuleRender::SwitchBloomActivation()
{
	bloomActivation = (bloomActivation + 1) % 2;
}

inline void ModuleRender::ToggleShadows()
{
	renderShadows = !renderShadows;
}

inline GLuint ModuleRender::GetRenderedTexture() const
{
	return renderedTexture[1];
}

inline BatchManager* ModuleRender::GetBatchManager() const
{
	return batchManager;
}

inline bool ModuleRender::IsObjectInsideFrustrum(const GameObject* gameObject)
{
	return gameObjectsInFrustrum.find(gameObject) != gameObjectsInFrustrum.end();
}

inline float ModuleRender::GetObjectDistance(const GameObject* gameObject)
{
	return objectsInFrustrumDistances[gameObject];
}