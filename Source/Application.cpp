#include "StdAfx.h"

#include "Application.h"

#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "ModuleAudio.h"
#include "ModuleCamera.h"
#include "ModuleCommand.h"
#include "ModuleDebugDraw.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"
#include "ModuleProgram.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleUI.h"
#include "ModuleWindow.h"
#include "ModuleNavigation.h"

#include "ScriptFactory.h"

#include "Scheduler.h"

#include "Defines/FramerateDefines.h"

constexpr int FRAMES_BUFFER = 50;

Application::Application() :
	maxFramerate(MAX_FRAMERATE),
	debuggingGame(false),
	editorPlayState(PlayState::STOPPED),
	closeGame(false)
{
	modules.resize(static_cast<int>(ModuleType::LAST));
	modules[static_cast<int>(ModuleToEnum<ModuleWindow>::value)] = std::make_unique<ModuleWindow>();
	modules[static_cast<int>(ModuleToEnum<ModuleFileSystem>::value)] = std::make_unique<ModuleFileSystem>();
	modules[static_cast<int>(ModuleToEnum<ModuleEditor>::value)] = std::make_unique<ModuleEditor>();
	modules[static_cast<int>(ModuleToEnum<ModuleInput>::value)] = std::make_unique<ModuleInput>();
	modules[static_cast<int>(ModuleToEnum<ModuleProgram>::value)] = std::make_unique<ModuleProgram>();
	modules[static_cast<int>(ModuleToEnum<ModuleCamera>::value)] = std::make_unique<ModuleCamera>();
	modules[static_cast<int>(ModuleToEnum<ModuleAudio>::value)] = std::make_unique<ModuleAudio>();
	modules[static_cast<int>(ModuleToEnum<ModuleScene>::value)] = std::make_unique<ModuleScene>();
	modules[static_cast<int>(ModuleToEnum<ModulePhysics>::value)] = std::make_unique<ModulePhysics>();
	modules[static_cast<int>(ModuleToEnum<ModuleNavigation>::value)] = std::make_unique<ModuleNavigation>();
	modules[static_cast<int>(ModuleToEnum<ModulePlayer>::value)] = std::make_unique<ModulePlayer>();
	modules[static_cast<int>(ModuleToEnum<ModuleRender>::value)] = std::make_unique<ModuleRender>();
	modules[static_cast<int>(ModuleToEnum<ModuleUI>::value)] = std::make_unique<ModuleUI>();
	modules[static_cast<int>(ModuleToEnum<ModuleResources>::value)] = std::make_unique<ModuleResources>();
	modules[static_cast<int>(ModuleToEnum<ModuleDebugDraw>::value)] = std::make_unique<ModuleDebugDraw>();
	modules[static_cast<int>(ModuleToEnum<ModuleCommand>::value)] = std::make_unique<ModuleCommand>();
}

Application::~Application()
{
	modules.clear();
}

bool Application::Init()
{
	scriptFactory = std::make_unique<ScriptFactory>();
	scriptFactory->Init();

	scheduler = std::make_unique<Scheduler>();

	for (const std::unique_ptr<Module>& module : modules)
	{
		if (!module->Init())
		{
			return false;
		}
	}

	return true;
}

bool Application::Start()
{
	appTimer.Start();

	for (const std::unique_ptr<Module>& module : modules)
	{
		if (!module->Start())
		{
			return false;
		}
	}

	return true;
}

UpdateStatus Application::Update()
{
	if (closeGame == true)
	{
		return UpdateStatus::UPDATE_STOP;
	}

	bool playMode = editorPlayState != PlayState::STOPPED;
	float ms = playMode ? onPlayTimer.Read() : appTimer.Read();

	for (const std::unique_ptr<Module>& module : modules)
	{
		UpdateStatus result = module->PreUpdate();
		if (result != UpdateStatus::UPDATE_CONTINUE)
		{
			return result;
		}
	}

	for (const std::unique_ptr<Module>& module : modules)
	{
		UpdateStatus result = module->Update();
		if (result != UpdateStatus::UPDATE_CONTINUE)
		{
			return result;
		}
	}

	for (const std::unique_ptr<Module>& module : modules)
	{
		UpdateStatus result = module->PostUpdate();
		if (result != UpdateStatus::UPDATE_CONTINUE)
		{
			return result;
		}
	}

	scheduler->RunTasks();

	float dt = playMode ? onPlayTimer.Read() - ms : appTimer.Read() - ms;

	if (dt < 1000.0f / GetMaxFrameRate())
	{
		SDL_Delay((Uint32) (1000.0f / GetMaxFrameRate() - dt));
	}

	deltaTime = playMode ? (onPlayTimer.Read() - ms) / 1000.0f : (appTimer.Read() - ms) / 1000.0f;

	return UpdateStatus::UPDATE_CONTINUE;
}

bool Application::CleanUp()
{
	std::ranges::reverse_view reverseModules = std::ranges::reverse_view{ modules };
	for (const std::unique_ptr<Module>& module : reverseModules)
	{
		if (!module->CleanUp())
		{
			return false;
		}
	}

	return true;
}

void Application::OnPlay()
{
	editorPlayState = PlayState::RUNNING;
	if (!GetModule<ModulePlayer>()->LoadNewPlayer())
	{
		editorPlayState = PlayState::STOPPED;
		LOG_WARNING("Player could not be loaded, game not starting");
		return;
		onPlayTimer.Stop();
	}

	onPlayTimer.Start();
	// Active Scripts
	GetModule<ModuleScene>()->OnPlay();
}

void Application::OnPause()
{
	if (GetPlayState() == PlayState::RUNNING)
	{
		editorPlayState = PlayState::PAUSED;
		GetModule<ModuleInput>()->SetShowCursor(true);
		GetModule<ModuleCamera>()->SetSelectedCamera(-1);
		GetModule<ModuleAudio>()->Suspend();
	}
	else if (GetPlayState() == PlayState::PAUSED)
	{
		editorPlayState = PlayState::RUNNING;
		GetModule<ModuleInput>()->SetShowCursor(false);
		GetModule<ModuleCamera>()->SetSelectedCamera(-1);
		GetModule<ModuleAudio>()->WakeUp();
	}
}

void Application::OnStop()
{
	editorPlayState = PlayState::STOPPED;
	GetModule<ModuleInput>()->SetShowCursor(true);
	GetModule<ModulePlayer>()->UnloadNewPlayer();
	onPlayTimer.Stop();
	GetModule<ModuleScene>()->OnStop();
}


void Application::ScheduleTask(std::function<void(void)>&& taskToSchedule)
{
	if (scheduler != nullptr)
	{
		scheduler->ScheduleTask(std::move(taskToSchedule));
	}
	else
	{
		// should never happen, just in case
		LOG_ERROR("Scheduler not initialized!!");
	}
}
