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
#include "ScriptFactory.h"

#include "Defines/FramerateDefines.h"

#include <ranges>

constexpr int FRAMES_BUFFER = 50;

Application::Application() : maxFramerate(MAX_FRAMERATE), debuggingGame(false), isOnPlayMode(false), closeGame(false)
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
#ifndef ENGINE
	isOnPlayMode = true;
#endif // !ENGINE

	scriptFactory = std::make_unique<ScriptFactory>();
	scriptFactory->Init();

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

	bool playMode = isOnPlayMode;
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

	float dt = playMode ? onPlayTimer.Read() - ms : appTimer.Read() - ms;

	if (dt < 1000.0f / GetMaxFrameRate())
	{
		SDL_Delay((Uint32)(1000.0f / GetMaxFrameRate() - dt));
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
	onPlayTimer.Start();
	isOnPlayMode = true;
	ModulePlayer* player = GetModule<ModulePlayer>();
	player->LoadNewPlayer();
	if (!player->GetPlayer())
	{
		isOnPlayMode = false;
	}

	// Active Scripts
	GetModule<ModuleScene>()->OnPlay();
}

void Application::OnStop()
{
	isOnPlayMode = false;
	GetModule<ModuleInput>()->SetShowCursor(true);
	GetModule<ModulePlayer>()->UnloadNewPlayer();
	onPlayTimer.Stop();
	GetModule<ModuleScene>()->OnStop();
}

void Application::OnPause()
{
	
}
