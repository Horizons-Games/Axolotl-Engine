#pragma once

#include "Module.h"
#include "Timer/Timer.h"

class ModuleRender;
class ModuleWindow;
class ModuleInput;
class ModuleProgram;
class ModuleTexture;
class ModuleFileSystem;
class ModuleResources;
class ModuleScene;
class ModuleCamera;
class ModuleDebugDraw;
#ifndef ENGINE
class ModulePlayer;
#endif //ENGINE
class ModuleEditor;

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

	bool IsDebuggingGame() const;
	void SetDebuggingGame(bool debuggingGame);
	void SwitchDebuggingGame();

public:
	ModuleScene* scene;
	ModuleFileSystem* fileSystem;
	ModuleRender* renderer;
	ModuleWindow* window;
	ModuleInput* input;
	ModuleProgram* program;
	ModuleResources* resources;
	ModuleCamera* camera;
#ifndef ENGINE
	ModulePlayer* player;
#endif // ENGINE
	ModuleDebugDraw* debug;
	ModuleEditor* editor;

private:
	std::vector<std::unique_ptr<Module> > modules;
	std::unique_ptr<Timer> appTimer;

	int maxFramerate;
	float deltaTime = 0.f;
	bool debuggingGame;
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

inline bool Application::IsDebuggingGame() const
{
	return debuggingGame;
}

inline void Application::SetDebuggingGame(bool debuggingGame)
{
	this->debuggingGame = debuggingGame;
}

inline void Application::SwitchDebuggingGame()
{
	debuggingGame = !debuggingGame;
}
