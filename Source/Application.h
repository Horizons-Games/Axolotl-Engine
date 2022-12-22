#pragma once

#include <vector>
#include <memory>

#include "Globals.h"
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

	inline float GetDeltaTime() const
	{
		return deltaTime;
	}

public:
	std::shared_ptr<ModuleRender> renderer;
	std::shared_ptr<ModuleWindow> window;
	std::shared_ptr<ModuleInput> input;
	std::shared_ptr<ModuleProgram> program;
	std::shared_ptr<ModuleDebugDraw> debug;
	std::shared_ptr<ModuleEditor> editor;
	std::shared_ptr<ModuleEngineCamera> engineCamera;
	std::shared_ptr<ModuleTexture> textures;

private:
	std::vector<std::shared_ptr<Module> > modules;
	std::unique_ptr<Timer> appTimer;

	int maxFramerate;
	float deltaTime = 0.f;
};

extern std::unique_ptr<Application> App;
