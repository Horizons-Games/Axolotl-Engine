#pragma once
#pragma warning (disable: 26495)

#include "Module.h"
#include "DataStructures/Quadtree.h"
#include "GL/glew.h"
#include <DataModels/Batch/BatchManager.h>

#include <unordered_map>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class Skybox;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender() override;

	bool Init() override;
	bool Start() override;
	bool CleanUp() override;

	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();

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

	void* context;
	float4 backgroundColor;

	BatchManager* batchManager;

	unsigned vbo;
	unsigned uboCamera;

	const std::vector<std::string> modelTypes;
	std::unordered_map<GeometryBatch*, std::vector<ComponentMeshRenderer*>> renderMap;

	//should this be here?
	std::unique_ptr<Skybox> skybox;

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