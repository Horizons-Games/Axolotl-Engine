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
class ModuleAudio;
class ModulePlayer;
class ModuleEditor;

class ScriptFactory;

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
	void OnStop();
	void OnPause();

	void SetMaxFrameRate(int maxFrames);
	int GetMaxFrameRate() const;
	float GetDeltaTime() const;
	bool IsOnPlayMode() const;

	bool IsDebuggingGame() const;
	void SetDebuggingGame(bool debuggingGame);
	void SetIsOnPlayMode(bool newIsOnPlayMode);
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
	ModuleAudio* audio;
	ModulePlayer* player;
	ModuleDebugDraw* debug;
	ModuleEditor* editor;
	
	std::unique_ptr<ScriptFactory> scriptFactory;

private:
	std::vector<std::unique_ptr<Module> > modules;
	Timer appTimer;
	Timer onPlayTimer;

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

inline bool Application::IsOnPlayMode() const
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

inline void Application::SetIsOnPlayMode(bool newIsOnPlayMode)
{
	isOnPlayMode = newIsOnPlayMode;
}

inline void Application::SwitchDebuggingGame()
{
	debuggingGame = !debuggingGame;
}
