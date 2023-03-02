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
class ModuleCommand;

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
	ModuleEngineCamera* engineCamera;
	ModuleResources* resources;
	ModuleCommand* command;
private:
	std::vector<std::unique_ptr<Module> > modules;
	std::unique_ptr<Timer> appTimer;

	int maxFramerate;
	float deltaTime = 0.f;
};

extern std::unique_ptr<Application> App;

inline void Application::SetMaxFrameRate(int maxFrames)
{
	maxFramerate = maxFrames;
}

inline int Application::GetMaxFrameRate() const
{
	return maxFramerate;
}

inline float Application::GetDeltaTime() const
{
	return deltaTime;
}
