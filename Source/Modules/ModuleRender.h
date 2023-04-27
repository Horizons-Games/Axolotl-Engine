#pragma once
#include "Module.h"
#include "DataStructures/Quadtree.h"
#include "GL/glew.h"

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

	void FillRenderList(const Quadtree* quadtree);
	void AddToRenderList(GameObject* gameObject);

	bool IsSupportedPath(const std::string& modelPath);
	void DrawQuadtree(const Quadtree* quadtree);

	//const std::vector<const GameObject*> GetGameObjectsToDraw() const;

private:
	void UpdateProgram();
	bool CheckIfTransparent(const GameObject* gameObject);
	void DrawGameObject(const GameObject* gameObject);
	void DrawSelectedHighlightGameObject(GameObject* gameObject);
	void DrawSelectedAndChildren(GameObject* gameObject);
	void DrawHighlight(GameObject* gameObject);

	void* context;
	float4 backgroundColor;

	unsigned vbo;
	
	std::vector<const GameObject*> opaqueGOToDraw;
	std::map<float, const GameObject*> transparentGOToDraw;
	//to avoid gameobjects being drawn twice
	std::vector<unsigned long long> drawnGameObjects;
	const std::vector<std::string> modelTypes;

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