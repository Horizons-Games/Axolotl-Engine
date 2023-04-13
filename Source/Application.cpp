#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleProgram.h"
#include "ModuleDebugDraw.h"
#include "ModuleEditor.h"
#include "ModuleEngineCamera.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/ModuleResources.h"
#include "ModuleScene.h"


constexpr int FRAMES_BUFFER = 50;

Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules.push_back(std::unique_ptr<ModuleFileSystem>(fileSystem = new ModuleFileSystem()));
	modules.push_back(std::unique_ptr<ModuleWindow>(window = new ModuleWindow()));
	modules.push_back(std::unique_ptr<ModuleEditor>(editor = new ModuleEditor()));
	modules.push_back(std::unique_ptr<ModuleInput>(input = new ModuleInput()));
	modules.push_back(std::unique_ptr<ModuleProgram>(program = new ModuleProgram()));	
	modules.push_back(std::unique_ptr<ModuleResources>(resources = new ModuleResources()));
	modules.push_back(std::unique_ptr<ModuleEngineCamera>(engineCamera = new ModuleEngineCamera()));
	modules.push_back(std::unique_ptr<ModuleScene>(scene = new ModuleScene()));
	modules.push_back(std::unique_ptr<ModuleRender>(renderer = new ModuleRender()));
	modules.push_back(std::unique_ptr<ModuleDebugDraw>(debug = new ModuleDebugDraw()));

	appTimer = std::make_unique<Timer>();
	maxFramerate = MAX_FRAMERATE;
}

Application::~Application()
{
	modules.clear();
}

bool Application::Init()
{
	bool ret = true;

	for (int i = 0; i < modules.size() && ret; ++i)
		ret = modules[i]->Init();

	return ret;
}

bool Application::Start()
{
	bool ret = true;

	appTimer->Start();

	for (int i = 0; i < modules.size() && ret; ++i)
		ret = modules[i]->Start();

	return ret;
}

update_status Application::Update()
{
	float ms = appTimer->Read();

	update_status ret = update_status::UPDATE_CONTINUE;

	for (int i = 0; i < modules.size() && ret == update_status::UPDATE_CONTINUE; ++i)
		ret = modules[i]->PreUpdate();

	for (int i = 0; i < modules.size() && ret == update_status::UPDATE_CONTINUE; ++i)
		ret = modules[i]->Update();

	for (int i = 0; i < modules.size() && ret == update_status::UPDATE_CONTINUE; ++i)
		ret = modules[i]->PostUpdate();

	float dt = (appTimer->Read() - ms) / 1000.0f;

	if (dt < 1000.0f / GetMaxFrameRate())
	{
		SDL_Delay((Uint32)(1000.0f / GetMaxFrameRate() - dt));
	}

	deltaTime = (appTimer->Read() - ms) / 1000.0f;

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (int i = (int)(modules.size() - 1); i >= 0; --i)
		ret = modules[i]->CleanUp();

	return ret;
}
