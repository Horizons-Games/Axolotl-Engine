#pragma once

#include "Module.h"
#include "Timer/Timer.h"

#include "Enums/ModuleType.h"
#include "Enums/UpdateStatus.h"

class ScriptFactory;
class Scheduler;

class Application
{
public:
	Application();
	~Application();

	bool Init();
	bool Start();
	UpdateStatus Update();
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

	// Add a Schedulable object (right now only a functor) that will be executed at the end of each frame
	// Optionally, set the frame delay until the object is executed
	void ScheduleTask(std::function<void(void)>&& taskToSchedule, std::uint16_t frameDelay = 0U);

private:
	std::unique_ptr<ScriptFactory> scriptFactory;
	std::unique_ptr<Scheduler> scheduler;

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
