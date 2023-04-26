#pragma once
#include "Application.h"

#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "ModuleProgram.h"
#include "ModuleCamera.h"
#include "ModuleAudio.h"
#include "ModuleUI.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
#include "ModuleEditor.h"
#include "ModulePlayer.h"
#include "ScriptFactory.h"

#include "Enums/ModuleType.h"

#include <ranges>

constexpr int FRAMES_BUFFER = 50;

Application::Application() : appTimer(Timer()), maxFramerate(MAX_FRAMERATE), debuggingGame(false),
isOnPlayMode(false), onPlayTimer(Timer())
{
	modules.reserve(static_cast<int>(ModuleType::LAST));
	modules[IndexOfModule<ModuleWindow>::value] = std::make_unique<ModuleWindow>();
	modules[IndexOfModule<ModuleFileSystem>::value] = std::make_unique<ModuleFileSystem>();
	modules[IndexOfModule<ModuleEditor>::value] = std::make_unique<ModuleEditor>();
	modules[IndexOfModule<ModuleInput>::value] = std::make_unique<ModuleInput>();
	modules[IndexOfModule<ModuleProgram>::value] = std::make_unique<ModuleProgram>();
	modules[IndexOfModule<ModuleCamera>::value] = std::make_unique<ModuleCamera>();
	modules[IndexOfModule<ModuleAudio>::value] = std::make_unique<ModuleAudio>();
	modules[IndexOfModule<ModuleScene>::value] = std::make_unique<ModuleScene>();
	modules[IndexOfModule<ModulePlayer>::value] = std::make_unique<ModulePlayer>();
	modules[IndexOfModule<ModuleRender>::value] = std::make_unique<ModuleRender>();
	modules[IndexOfModule<ModuleUI>::value] = std::make_unique<ModuleUI>();
	modules[IndexOfModule<ModuleResources>::value] = std::make_unique<ModuleResources>();
	modules[IndexOfModule<ModuleDebugDraw>::value] = std::make_unique<ModuleDebugDraw>();
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
		SDL_Delay((Uint32)(1000.0f / GetMaxFrameRate() - dt));
	}

#ifdef ENGINE
	(isOnPlayMode) ?
		deltaTime = (onPlayTimer.Read() - ms) / 1000.0f : deltaTime = (appTimer.Read() - ms) / 1000.0f;
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

	//Active Scripts
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
