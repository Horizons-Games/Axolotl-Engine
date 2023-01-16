#pragma once
#include "Module.h"
#include "Globals.h"
#include "3DModels/Model.h"
#include "Quadtree.h"
#include "Math/float4x4.h"
#include "GL/glew.h"

#include <SDL.h>

#include <list>

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

	void SetShaders(const std::string& vertexShader, const std::string& fragmentShader);

	void SetBackgroundColor(float4 color);
	float4 GetBackgroundColor() const;

	std::shared_ptr<Model> GetModel(unsigned pos) const; // This method should disappear
	const int GetModelCount() const; // This method should disappear

	unsigned int GetRenderedTexture() const;
	const std::string& GetVertexShader() const;
	const std::string& GetFragmentShader() const;

	void DrawScene(Quadtree* quadtree);
	
	bool LoadModel(const char* path); // This method should disappear
	bool AnyModelLoaded(); // This method should disappear

	bool IsSupportedPath(const std::string& modelPath);

private:
	void DrawGameObject(std::shared_ptr<GameObject>& gameObject);
	void UpdateProgram();

	void* context;
	float4 backgroundColor;

	unsigned vbo;
	
	std::vector<std::shared_ptr<Model> > models; // This vector should disappear

	std::shared_ptr<Skybox> skybox;

	std::vector<std::shared_ptr<GameObject>> gameObjects;
	const std::vector<std::string> modelTypes = { "FBX" };

	GLuint frameBuffer = 0;
	GLuint renderedTexture = 0;
	GLuint depthRenderBuffer = 0;

	std::string vertexShader = "default_vertex.glsl";
	std::string fragmentShader = "default_fragment.glsl";

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

inline std::shared_ptr<Model> ModuleRender::GetModel(unsigned pos) const
{
	return models[pos];
}

inline const int ModuleRender::GetModelCount() const
{
	return models.size();
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

