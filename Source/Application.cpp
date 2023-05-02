#pragma once
#include "Application.h"

#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "ModuleAudio.h"
#include "ModuleCamera.h"
#include "ModuleDebugDraw.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModulePlayer.h"
#include "ModuleProgram.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleUI.h"
#include "ModuleWindow.h"
#include "ScriptFactory.h"

#include <ranges>

constexpr int FRAMES_BUFFER = 50;

Application::Application() : maxFramerate(MAX_FRAMERATE), debuggingGame(false), isOnPlayMode(false)
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
	modules[static_cast<int>(ModuleToEnum<ModulePlayer>::value)] = std::make_unique<ModulePlayer>();
	modules[static_cast<int>(ModuleToEnum<ModuleRender>::value)] = std::make_unique<ModuleRender>();
	modules[static_cast<int>(ModuleToEnum<ModuleUI>::value)] = std::make_unique<ModuleUI>();
	modules[static_cast<int>(ModuleToEnum<ModuleResources>::value)] = std::make_unique<ModuleResources>();
	modules[static_cast<int>(ModuleToEnum<ModuleDebugDraw>::value)] = std::make_unique<ModuleDebugDraw>();
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

update_status Application::Update()
{
	float ms;
#ifdef ENGINE
	(isOnPlayMode) ? ms = onPlayTimer.Read() : ms = appTimer.Read();
#else
	ms = appTimer.Read();
#endif // ENGINE

	for (const std::unique_ptr<Module>& module : modules)
	{
		update_status result = module->PreUpdate();
		if (result != update_status::UPDATE_CONTINUE)
		{
			return result;
		}
	}

	for (const std::unique_ptr<Module>& module : modules)
	{
		update_status result = module->Update();
		if (result != update_status::UPDATE_CONTINUE)
		{
			return result;
		}
	}

	for (const std::unique_ptr<Module>& module : modules)
	{
		update_status result = module->PostUpdate();
		if (result != update_status::UPDATE_CONTINUE)
		{
			return result;
		}
	}

	float dt;
#ifdef ENGINE
	(isOnPlayMode) ? dt = (onPlayTimer.Read() - ms) / 1000.0f : dt = (appTimer.Read() - ms) / 1000.0f;
#else
	dt = (appTimer.Read() - ms) / 1000.0f;
#endif // ENGINE

	if (dt < 1000.0f / GetMaxFrameRate())
	{
		SDL_Delay((Uint32) (1000.0f / GetMaxFrameRate() - dt));
	}

#ifdef ENGINE
	(isOnPlayMode) ? deltaTime = (onPlayTimer.Read() - ms) / 1000.0f : deltaTime = (appTimer.Read() - ms) / 1000.0f;
#else
	deltaTime = (appTimer.Read() - ms) / 1000.0f;
#endif // ENGINE

	return update_status::UPDATE_CONTINUE;
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
	if (!player->IsLoadPlayer())
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
	GetModule<ModuleScene>()->OnPause();
}
