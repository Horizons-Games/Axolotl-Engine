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

#include <ranges>

constexpr int FRAMES_BUFFER = 50;

Application::Application() : appTimer(Timer()), maxFramerate(MAX_FRAMERATE), debuggingGame(false),
isOnPlayMode(false), onPlayTimer(Timer())
{
	// Order matters: they will Init/start/update in this order
	modules.insert({ ModuleToEnum<ModuleWindow>::value, std::make_unique<ModuleWindow>() });
	modules.insert({ ModuleToEnum<ModuleFileSystem>::value, std::make_unique<ModuleFileSystem>() });
	modules.insert({ ModuleToEnum<ModuleEditor>::value, std::make_unique<ModuleEditor>() });
	modules.insert({ ModuleToEnum<ModuleInput>::value, std::make_unique<ModuleInput>() });
	modules.insert({ ModuleToEnum<ModuleProgram>::value, std::make_unique<ModuleProgram>() });
	modules.insert({ ModuleToEnum<ModuleCamera>::value, std::make_unique<ModuleCamera>() });
	modules.insert({ ModuleToEnum<ModuleAudio>::value, std::make_unique<ModuleAudio>() });
	modules.insert({ ModuleToEnum<ModuleScene>::value, std::make_unique<ModuleScene>() });
	modules.insert({ ModuleToEnum<ModulePlayer>::value, std::make_unique<ModulePlayer>() });
	modules.insert({ ModuleToEnum<ModuleRender>::value, std::make_unique<ModuleRender>() });
	modules.insert({ ModuleToEnum<ModuleUI>::value, std::make_unique<ModuleUI>() });
	modules.insert({ ModuleToEnum<ModuleResources>::value, std::make_unique<ModuleResources>() });
	modules.insert({ ModuleToEnum<ModuleDebugDraw>::value, std::make_unique<ModuleDebugDraw>() });
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

	for (const auto& enumAndModule : modules)
	{
		if (!enumAndModule.second->Init())
		{
			return false;
		}
	}

	return true;
}

bool Application::Start()
{
	appTimer.Start();

	for (const auto& enumAndModule : modules)
	{
		if (!enumAndModule.second->Start())
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

	for (const auto& enumAndModule : modules)
	{
		update_status result = enumAndModule.second->PreUpdate();
		if (result != update_status::UPDATE_CONTINUE)
		{
			return result;
		}
	}

	for (const auto& enumAndModule : modules)
	{
		update_status result = enumAndModule.second->Update();
		if (result != update_status::UPDATE_CONTINUE)
		{
			return result;
		}
	}

	for (const auto& enumAndModule : modules)
	{
		update_status result = enumAndModule.second->PostUpdate();
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
	for (const auto& enumAndModule : reverseModules)
	{
		if (!enumAndModule.second->CleanUp())
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
