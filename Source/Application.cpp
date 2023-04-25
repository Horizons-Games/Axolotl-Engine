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

constexpr int FRAMES_BUFFER = 50;

Application::Application() : appTimer(Timer()), maxFramerate(MAX_FRAMERATE), debuggingGame(false), 
								isOnPlayMode(false), onPlayTimer(Timer())
{
	// Order matters: they will Init/start/update in this order
	modules.push_back(std::unique_ptr<ModuleWindow>(window = new ModuleWindow()));
	modules.push_back(std::unique_ptr<ModuleFileSystem>(fileSystem = new ModuleFileSystem()));
	modules.push_back(std::unique_ptr<ModuleEditor>(editor = new ModuleEditor()));
	modules.push_back(std::unique_ptr<ModuleInput>(input = new ModuleInput()));
	modules.push_back(std::unique_ptr<ModuleProgram>(program = new ModuleProgram()));	
	modules.push_back(std::unique_ptr<ModuleCamera>(camera = new ModuleCamera()));
	modules.push_back(std::unique_ptr<ModuleAudio>(audio = new ModuleAudio()));
	modules.push_back(std::unique_ptr<ModuleScene>(scene = new ModuleScene()));
	modules.push_back(std::unique_ptr<ModulePlayer>(player = new ModulePlayer()));
	modules.push_back(std::unique_ptr<ModuleRender>(renderer = new ModuleRender()));
	modules.push_back(std::unique_ptr<ModuleUI>(userInterface = new ModuleUI()));
	modules.push_back(std::unique_ptr<ModuleResources>(resources = new ModuleResources()));
	modules.push_back(std::unique_ptr<ModuleDebugDraw>(debug = new ModuleDebugDraw()));
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
	bool ret = true;

	for (int i = 0; i < modules.size() && ret; ++i)
		ret = modules[i]->Init();

	return ret;
}

bool Application::Start()
{
	bool ret = true;

	appTimer.Start();

	for (int i = 0; i < modules.size() && ret; ++i)
		ret = modules[i]->Start();

	return ret;
}

update_status Application::Update()
{
	bool playModeStart = isOnPlayMode;
	float ms;

#ifdef ENGINE
	(playModeStart) ? ms = onPlayTimer.Read() : ms = appTimer.Read();
	(isOnPlayMode) ? ms = onPlayTimer.Read() : ms = appTimer.Read();
#else
	ms = appTimer.Read();
#endif // ENGINE

	update_status ret = update_status::UPDATE_CONTINUE;

	for (int i = 0; i < modules.size() && ret == update_status::UPDATE_CONTINUE; ++i)
		ret = modules[i]->PreUpdate();

	for (int i = 0; i < modules.size() && ret == update_status::UPDATE_CONTINUE; ++i)
		ret = modules[i]->Update();

	for (int i = 0; i < modules.size() && ret == update_status::UPDATE_CONTINUE; ++i)
		ret = modules[i]->PostUpdate();

	float dt;
	
#ifdef ENGINE
	(playModeStart) ? dt = (onPlayTimer.Read() - ms) / 1000.0f : dt = (appTimer.Read() - ms) / 1000.0f;
	(isOnPlayMode) ? dt = (onPlayTimer.Read() - ms) / 1000.0f : dt = (appTimer.Read() - ms) / 1000.0f;
#else
	dt = (appTimer.Read() - ms) / 1000.0f;
#endif // ENGINE


	if (dt < 1000.0f / GetMaxFrameRate())
	{
		SDL_Delay((Uint32)(1000.0f / GetMaxFrameRate() - dt));
	}


#ifdef ENGINE
	(playModeStart) ? deltaTime = (onPlayTimer.Read() - ms) / 1000.0f : deltaTime = (appTimer.Read() - ms) / 1000.0f;
	(isOnPlayMode) ?
		deltaTime = (onPlayTimer.Read() - ms) / 1000.0f : deltaTime = (appTimer.Read() - ms) / 1000.0f;
#else
	deltaTime = (appTimer.Read() - ms) / 1000.0f;
#endif // ENGINE
	

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (int i = (int)(modules.size() - 1); i >= 0; --i)
		ret = modules[i]->CleanUp();

	return ret;
}

void Application::OnPlay()
{
	onPlayTimer.Start();
	isOnPlayMode = true;
	player->LoadNewPlayer();
	if (!player->IsLoadPlayer())
	{
		isOnPlayMode = false;
	}
	
	//Active Scripts
	scene->OnPlay();
}

void Application::OnStop()
{
	isOnPlayMode = false;
	input->SetShowCursor(true);
	player->UnloadNewPlayer();
	onPlayTimer.Stop();
	scene->OnStop();
}

void Application::OnPause()
{
	scene->OnPause();
}
