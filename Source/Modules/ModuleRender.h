#pragma once
#include "Module.h"
#include "DataStructures/Quadtree.h"
#include "GL/glew.h"
#include <DataModels/Batch/BatchManager.h>

#include <unordered_map>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class Skybox;
class Program;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender() override;

	bool Init() override;
	bool Start() override;
	bool CleanUp() override;

	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;

	void WindowResized(unsigned width, unsigned height);
	void UpdateBuffers(unsigned width, unsigned height);

	void SetBackgroundColor(float4 color);
	float4 GetBackgroundColor() const;

	unsigned int GetRenderedTexture() const;
	const std::string& GetVertexShader() const;
	const std::string& GetFragmentShader() const;

	BatchManager* GetBatchManager() const;

	void FillRenderList(const Quadtree* quadtree);
	void AddToRenderList(GameObject* gameObject);

	bool IsSupportedPath(const std::string& modelPath);
	void DrawQuadtree(const Quadtree* quadtree);

private:
	void UpdateProgram();
	
	bool CheckIfTransparent(const GameObject* gameObject);

	void DrawGameObject(const GameObject* gameObject);
	void DrawSelectedHighlightGameObject(GameObject* gameObject);
	void DrawSelectedAndChildren(GameObject* gameObject);
	void DrawHighlight(GameObject* gameObject);

	void BindCameraToProgram(Program* program);

	void* context;
	float4 backgroundColor;

	BatchManager* batchManager;

	unsigned uboCamera;
	unsigned vbo;
	
	std::vector<ComponentMeshRenderer*> opaqueGOToDraw;
	std::map<float, ComponentMeshRenderer*> transparentGOToDraw;
	std::vector<ComponentMeshRenderer*> transparentComponents;
	//to avoid gameobjects being drawn twice
	std::vector<unsigned long long> drawnGameObjects;
	const std::vector<std::string> modelTypes;
	std::unordered_map<GeometryBatch*, std::vector<ComponentMeshRenderer*>> renderMapOpaque;
	std::unordered_map<GeometryBatch*, std::vector<ComponentMeshRenderer*>> renderMapTransparent;

	GLuint frameBuffer;
	GLuint renderedTexture;
	GLuint depthStencilRenderbuffer;

	std::string vertexShader;
	std::string fragmentShader;

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

inline unsigned int ModuleRender::GetRenderedTexture() const
{
	return renderedTexture;
}

inline const std::string& ModuleRender::GetVertexShader() const
{
	return vertexShader;
}

inline const std::string& ModuleRender::GetFragmentShader() const
{
	return fragmentShader;
}

inline BatchManager* ModuleRender::GetBatchManager() const
{
	return batchManager;
}