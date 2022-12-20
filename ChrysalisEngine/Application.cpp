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

constexpr int FRAMES_BUFFER = 50;

Application::Application()
{
	fpsLog = std::vector<float>(FRAMES_BUFFER, 0.0f);
	msLog = std::vector<float>(FRAMES_BUFFER, 0.0f);

	// Order matters: they will Init/start/update in this order
	modules.push_back(window = new ModuleWindow());
	modules.push_back(editor = new ModuleEditor());
	modules.push_back(input = new ModuleInput());
	modules.push_back(program = new ModuleProgram()); 
	modules.push_back(renderer = new ModuleRender());
	modules.push_back(debug = new ModuleDebugDraw());
	modules.push_back(textures = new ModuleTexture());
	modules.push_back(engineCamera = new ModuleEngineCamera());

	appTimer = new Timer();
	maxFramerate = MAX_FRAMERATE;
}

Application::~Application()
{
	for(int i = 0; i < modules.size(); ++i)
        delete modules[i];

	fpsLog.clear();
	msLog.clear();
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
	this->fps = 1 / this->deltaTime;
	this->AddFrame(App->fps, App->deltaTime);

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (int i = modules.size() - 1; i >= 0; --i)
		ret = modules[i]->CleanUp();

	return ret;
}

void Application::AddFrame(int fps, float ms)
{
	for (unsigned i = 0; i < FRAMES_BUFFER - 1; ++i)
	{
		fpsLog[i] = fpsLog[i + 1];
		msLog[i] = msLog[i + 1];
	}

	fpsLog[FRAMES_BUFFER - 1] = fps;
	msLog[FRAMES_BUFFER - 1] = ms * 1000.0f;
}

void Application::SetMaxFrameRate(int maxFrames)
{
	this->maxFramerate = maxFrames;
}

int Application::GetMaxFrameRate() const
{
	return this->maxFramerate;
}
