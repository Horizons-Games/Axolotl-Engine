#pragma once

#include "Module.h"
#include "Timer/Timer.h"

#include "Enums/ModuleType_fwd.h"

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

	ScriptFactory* GetScriptFactory() const;
	template<typename M>
	M* GetModule();

private:	
	std::unique_ptr<ScriptFactory> scriptFactory;

	std::vector<std::unique_ptr<Module>> modules;
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

inline ScriptFactory* Application::GetScriptFactory() const
{
	return scriptFactory.get();
}

template<typename M>
M* Application::GetModule()
{
	int index = IndexOfModule<M>::value;
	return index >= 0 ? static_cast<M*>(modules[index].get()) : nullptr;
}
