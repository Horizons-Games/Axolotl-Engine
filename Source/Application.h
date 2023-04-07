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
class ModuleUI;
class ModulePlayer;
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

	void OnPlay();
	void OnStopPlay();
	void OnPause();

	void SetMaxFrameRate(int maxFrames);
	int GetMaxFrameRate() const;
	float GetDeltaTime() const;
	bool GetIsOnPlayMode() const;

	bool IsDebuggingGame() const;
	void SetDebuggingGame(bool debuggingGame);
	void SwitchDebuggingGame();

public:
	ModuleScene* scene;
	ModuleFileSystem* fileSystem;
	ModuleRender* renderer;
	ModuleUI* userInterface;
	ModuleWindow* window;
	ModuleInput* input;
	ModuleProgram* program;
	ModuleResources* resources;
	ModuleCamera* camera;
	ModulePlayer* player;
	ModuleDebugDraw* debug;
	ModuleEditor* editor;

private:
	std::vector<std::unique_ptr<Module> > modules;
	std::unique_ptr<Timer> appTimer;
	std::unique_ptr<Timer> onPlayTimer;

	int maxFramerate;
	float deltaTime = 0.f;
	bool debuggingGame;
	bool isOnPlayMode;
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

inline bool Application::GetIsOnPlayMode() const
{
	return isOnPlayMode;
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
