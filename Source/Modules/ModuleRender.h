#pragma once

#include "Module.h"

#include "DataModels/Batch/BatchManager.h"

#include "DataStructures/Quadtree.h"

#include "GL/glew.h"
#include "Math/float4.h"
#include "Module.h"

#include <map>
#include <unordered_map>
#include <unordered_set>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class Skybox;
class Program;
class Cubemap;
class GameObject;

class ModuleRender : public Module
{
public:

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
	void SetModeRender();
	float4 GetBackgroundColor() const;

	GLuint GetRenderedTexture() const;
	float GetObjectDistance(const GameObject* gameObject);

	BatchManager* GetBatchManager() const;

	void FillRenderList(const Quadtree* quadtree);
	void AddToRenderList(const GameObject* gameObject);

	bool IsObjectInsideFrustrum(const GameObject* gameObject);
	bool IsSupportedPath(const std::string& modelPath);

	void DrawQuadtree(const Quadtree* quadtree);

	// const std::vector<const GameObject*> GetGameObjectsToDraw() const;

private:
	void UpdateProgram();
	bool CheckIfTransparent(const GameObject* gameObject);

	void DrawHighlight(GameObject* gameObject);

	void BindCameraToProgram(Program* program);

	void* context;

	float4 backgroundColor;

	BatchManager* batchManager;

	unsigned uboCamera;
	unsigned vbo;

	unsigned modeRender;
	
	std::map<float, ComponentMeshRenderer*> transparentGOToDraw;
	std::vector<ComponentMeshRenderer*> transparentComponents;
	std::vector<unsigned long long> drawnGameObjects;

	const std::vector<std::string> modelTypes;

	std::unordered_set<const GameObject*> gameObjectsInFrustrum;
	std::unordered_map<const GameObject*, float> objectsInFrustrumDistances;

	std::unordered_map<GeometryBatch*, std::vector<ComponentMeshRenderer*>> renderMapOpaque;
	std::unordered_map<GeometryBatch*, std::vector<ComponentMeshRenderer*>> renderMapTransparent;

	GLuint frameBuffer;
	GLuint gPosition;
	GLuint gNormal;
	GLuint gDiffuse;
	GLuint gSpecular;
	GLuint depthTexture;

	friend class ModuleEditor;
};

inline void ModuleRender::SetBackgroundColor(float4 color)
{
	backgroundColor = color;
}
inline void ModuleRender::SetModeRender()
{
	if (modeRender == 4)
	{
		modeRender = 0;
	}
	else
	{
		modeRender++;
	}
}

inline float4 ModuleRender::GetBackgroundColor() const
{
	return backgroundColor;
}

inline GLuint ModuleRender::GetRenderedTexture() const
{
	return gDiffuse;
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