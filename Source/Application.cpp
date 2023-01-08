#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleProgram.h"
#include "ModuleDebugDraw.h"
#include "ModuleEditor.h"
#include "ModuleEngineCamera.h"
#include "ModuleTexture.h"
#include "FileSystem/ModuleFileSystem.h"
#include "ModuleScene.h"

constexpr int FRAMES_BUFFER = 50;

Application::Application()
{
	// Order matters: they will Init/start/update in this order
	modules.push_back(window = std::make_shared<ModuleWindow>());
	modules.push_back(editor = std::make_shared<ModuleEditor>());
	modules.push_back(input = std::make_shared<ModuleInput>());
	modules.push_back(program = std::make_shared<ModuleProgram>());
	modules.push_back(renderer = std::make_shared<ModuleRender>());
	modules.push_back(debug = std::make_shared<ModuleDebugDraw>());
	modules.push_back(textures = std::make_shared<ModuleTexture>());
	modules.push_back(engineCamera = std::make_shared<ModuleEngineCamera>());
	modules.push_back(fileSystem = std::make_shared<ModuleFileSystem>());
	modules.push_back(scene = std::make_shared<ModuleScene>());


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

	update_status ret = UPDATE_CONTINUE;

	for (int i = 0; i < modules.size() && ret == UPDATE_CONTINUE; ++i)
		ret = modules[i]->PreUpdate();

	for (int i = 0; i < modules.size() && ret == UPDATE_CONTINUE; ++i)
		ret = modules[i]->Update();

	for (int i = 0; i < modules.size() && ret == UPDATE_CONTINUE; ++i)
		ret = modules[i]->PostUpdate();

	float dt = (appTimer->Read() - ms) / 1000.0f;

	if (dt < 1000.0f / GetMaxFrameRate())
	{
		SDL_Delay(1000.0f / GetMaxFrameRate() - dt);
	}

	this->deltaTime = (appTimer->Read() - ms) / 1000.0f;

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (int i = modules.size() - 1; i >= 0; --i)
		ret = modules[i]->CleanUp();

	return ret;
}
