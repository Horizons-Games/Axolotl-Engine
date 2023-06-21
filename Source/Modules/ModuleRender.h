#pragma once

#include "Module.h"

#include "DataModels/Batch/BatchManager.h"

#include "DataStructures/Quadtree.h"

#include "GL/glew.h"
#include "Math/float4.h"

#include <unordered_map>
#include <unordered_set>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class Skybox;
class Program;
class Cubemap;
class GameObject;
class GBuffer;

class ModuleRender : public Module
{
public:

	enum ModeRender{
		POSITION = 1,
		NORMAL = 2,
		DIFFUSE = 3,
		SPECULAR = 4,
		LENGTH = 5
	};

	ModuleRender();
	~ModuleRender() override;

	bool Init() override;

	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;

	bool CleanUp() override;

	void WindowResized(unsigned width, unsigned height);
	void UpdateBuffers(unsigned width, unsigned height);

	void SetBackgroundColor(float4 color);
	void ChangeRenderMode();
	float4 GetBackgroundColor() const;

	GLuint GetRenderedTexture() const;
	float GetObjectDistance(const GameObject* gameObject);

	BatchManager* GetBatchManager() const;

	void FillRenderList(const Quadtree* quadtree);
	void AddToRenderList(const GameObject* gameObject);

	bool IsObjectInsideFrustrum(const GameObject* gameObject);

	void DrawQuadtree(const Quadtree* quadtree);

private:

	bool CheckIfTransparent(const GameObject* gameObject);

	void DrawHighlight(GameObject* gameObject);

	void BindCameraToProgram(Program* program);
	void BindCubemapToProgram(Program* program);

	void* context;

	float4 backgroundColor;

	BatchManager* batchManager;
	GBuffer* gBuffer;

	unsigned uboCamera;
	unsigned vbo;

	unsigned modeRender;
	
	std::unordered_set<const GameObject*> gameObjectsInFrustrum;
	std::unordered_map<const GameObject*, float> objectsInFrustrumDistances;

	GLuint frameBuffer;
	GLuint renderedTexture;
	
	GLuint depthStencilRenderBuffer;

	friend class ModuleEditor;
};

inline void ModuleRender::SetBackgroundColor(float4 color)
{
	backgroundColor = color;
}

inline void ModuleRender::ChangeRenderMode()
{
		int modeRenderAsInt = static_cast<int>(modeRender);
		modeRenderAsInt = (modeRenderAsInt + 1) % static_cast<int>(ModeRender::LENGTH);
		modeRender = static_cast<ModeRender>(modeRenderAsInt);
}

inline float4 ModuleRender::GetBackgroundColor() const
{
	return backgroundColor;
}

inline GLuint ModuleRender::GetRenderedTexture() const
{
	return renderedTexture;
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