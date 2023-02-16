#pragma once

#include "Module.h"
#include "Timer/Timer.h"

class ModuleRender;
class ModuleWindow;
class ModuleInput;
class ModuleProgram;
class ModuleDebugDraw;
class ModuleEditor;
class ModuleEngineCamera;
class ModuleTexture;
class ModuleFileSystem;
class ModuleResources;
class ModuleScene;

class Application
{
public:

	Application();
	~Application();

	bool Start();
	bool Init();
	update_status Update();
	bool CleanUp();

	void SetMaxFrameRate(int maxFrames);
	int GetMaxFrameRate() const;
	float GetDeltaTime() const;

	ModuleWindow* GetModuleWindow() const;
	ModuleEditor* GetModuleEditor() const;
	ModuleInput* GetModuleInput() const;
	ModuleProgram* GetModuleProgram() const;
	ModuleFileSystem* GetModuleFileSystem() const;
	ModuleResources* GetModuleResources() const;
	ModuleEngineCamera* GetModuleEngineCamera() const;
	ModuleScene* GetModuleScene() const;
	ModuleRender* GetModuleRender() const;
	ModuleDebugDraw* GetModuleDebugDraw() const;

private:
	std::vector<std::unique_ptr<Module> > modules;
	std::unique_ptr<Timer> appTimer;

	ModuleWindow* window;
	ModuleEditor* editor;
	ModuleInput* input;
	ModuleProgram* program;
	ModuleFileSystem* fileSystem;
	ModuleResources* resources;
	ModuleEngineCamera* engineCamera;
	ModuleScene* scene;
	ModuleRender* renderer;
	ModuleDebugDraw* debug;

	int maxFramerate;
	float deltaTime = 0.f;
};

extern std::unique_ptr<Application> App;

inline void Application::SetMaxFrameRate(int maxFrames)
{
	this->maxFramerate = maxFrames;
}

inline int Application::GetMaxFrameRate() const
{
	return maxFramerate;
}

inline float Application::GetDeltaTime() const
{
	return deltaTime;
}

inline ModuleWindow* Application::GetModuleWindow() const
{
	return window;
}

inline ModuleEditor* Application::GetModuleEditor() const
{
	return editor;
}

inline ModuleInput* Application::GetModuleInput() const
{
	return input;
}

inline ModuleProgram* Application::GetModuleProgram() const
{
	return program;
}

inline ModuleFileSystem* Application::GetModuleFileSystem() const
{
	return fileSystem;
}

inline ModuleResources* Application::GetModuleResources() const
{
	return resources;
}

inline ModuleEngineCamera* Application::GetModuleEngineCamera() const
{
	return engineCamera;
}

inline ModuleScene* Application::GetModuleScene() const
{
	return scene;
}

inline ModuleRender* Application::GetModuleRender() const
{
	return renderer;
}

inline ModuleDebugDraw* Application::GetModuleDebugDraw() const
{
	return debug;
}
