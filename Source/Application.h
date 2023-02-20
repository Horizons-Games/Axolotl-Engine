#pragma once

#include "Module.h"
#include "Timer/Timer.h"

class ModuleRender;
class ModuleWindow;
class ModuleInput;
class ModuleProgram;
class ModuleDebugDraw;
class ModuleEditor;
class ModuleCamera;
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

public:
	ModuleScene* scene;
	ModuleFileSystem* fileSystem;
	ModuleRender* renderer;
	ModuleWindow* window;
	ModuleInput* input;
	ModuleProgram* program;
	ModuleDebugDraw* debug;
	ModuleEditor* editor;
	ModuleCamera* engineCamera;
	ModuleResources* resources;
private:
	std::vector<std::unique_ptr<Module> > modules;
	std::unique_ptr<Timer> appTimer;

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
