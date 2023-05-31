#pragma once

#include "Module.h"
#include "Timer/Timer.h"

#include "Enums/ModuleType.h"

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
	void SetCloseGame(bool closeGameStatus);
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
	bool closeGame;
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
	isOnPlayMode ? OnPlay() : OnStop();
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
	int index = static_cast<int>(ModuleToEnum<M>::value);
	return static_cast<M*>(modules[index].get());
}

inline void Application::SetCloseGame(bool closeGameStatus)
{
	closeGame = closeGameStatus;
}
