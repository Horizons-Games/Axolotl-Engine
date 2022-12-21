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

	void AddFrame(int fps, float ms);

	void SetMaxFrameRate(int maxFrames);
	int GetMaxFrameRate() const;

public:
	std::shared_ptr<ModuleRender> renderer;
	std::shared_ptr<ModuleWindow> window;
	std::shared_ptr<ModuleInput> input;
	std::shared_ptr<ModuleProgram> program;
	std::shared_ptr<ModuleDebugDraw> debug;
	std::shared_ptr<ModuleEditor> editor;
	std::shared_ptr<ModuleEngineCamera> engineCamera;
	std::shared_ptr<ModuleTexture> textures;

	float deltaTime = 0.f;
	int fps = 0;

	std::vector<float> fpsLog;
	std::vector<float> msLog;

	std::unique_ptr<Timer> appTimer;

private:
	std::vector<std::shared_ptr<Module> > modules;

	int maxFramerate;
};

extern std::unique_ptr<Application> App;
