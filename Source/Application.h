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
	// For the record, it doesn't make sense for this to be in the Application layer
	// This enum only carries meaning in Editor/Engine mode, and thus should not be compiled in Game
	// But I don't feel like doing that big of a refactor
	enum class PlayState
	{
		RUNNING,
		PAUSED,
		STOPPED
	};

public:
	Application();
	~Application();

	bool Init();
	bool Start();
	UpdateStatus Update();
	bool CleanUp();

	void OnPlay();
	void OnPause();
	void OnStop();

	PlayState GetPlayState() const;

	void SetMaxFrameRate(int maxFrames);
	int GetMaxFrameRate() const;
	float GetDeltaTime() const;

	bool IsDebuggingGame() const;
	void SetDebuggingGame(bool debuggingGame);
	void SwitchDebuggingGame();

	ScriptFactory* GetScriptFactory() const;
	void SetCloseGame(bool closeGameStatus);
	template<typename M>
	M* GetModule();

	// Add a Schedulable object (right now only a functor) that will be executed at the end of each frame
	void ScheduleTask(std::function<void(void)>&& taskToSchedule);

private:
	std::unique_ptr<ScriptFactory> scriptFactory;
	std::unique_ptr<Scheduler> scheduler;

	std::vector<std::unique_ptr<Module>> modules;
	Timer appTimer;
	Timer onPlayTimer;

	int maxFramerate;
	float deltaTime = 0.f;
	bool debuggingGame;
	bool closeGame;

	PlayState editorPlayState;
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

inline Application::PlayState Application::GetPlayState() const
{
	// See comment in the enum class
#ifdef GAME
	return PlayState::RUNNING;
#else
	return editorPlayState;
#endif // GAME
}
