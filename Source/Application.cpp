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
	modules.emplace(ModuleType::WINDOW, std::make_unique<ModuleWindow>());
	modules.emplace(ModuleType::EDITOR, std::make_unique<ModuleEditor>());
	modules.emplace(ModuleType::INPUT, std::make_unique<ModuleInput>());
	modules.emplace(ModuleType::PROGRAM, std::make_unique<ModuleProgram>());
	modules.emplace(ModuleType::FILESYSTEM, std::make_unique<ModuleFileSystem>());
	modules.emplace(ModuleType::RESOURCES, std::make_unique<ModuleResources>());
	modules.emplace(ModuleType::CAMERA, std::make_unique<ModuleEngineCamera>());
	modules.emplace(ModuleType::SCENE, std::make_unique<ModuleScene>());
	modules.emplace(ModuleType::RENDER, std::make_unique<ModuleRender>());
	modules.emplace(ModuleType::DEBUGDRAW, std::make_unique<ModuleDebugDraw>());

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

	for (const std::pair<const ModuleType, std::unique_ptr<Module>>& moduleTypePair : modules)
		ret = moduleTypePair.second->Init();

	return ret;
}

bool Application::Start()
{
	bool ret = true;

	appTimer->Start();

	for (const std::pair<const ModuleType, std::unique_ptr<Module>>& moduleTypePair : modules)
		ret = moduleTypePair.second->Start();

	return ret;
}

update_status Application::Update()
{
	float ms = appTimer->Read();

	update_status ret = UPDATE_CONTINUE;

	for (auto it = std::begin(modules); it != std::end(modules) && ret == UPDATE_CONTINUE; ++it)
		ret = (*it).second->PreUpdate();

	for (auto it = std::begin(modules); it != std::end(modules) && ret == UPDATE_CONTINUE; ++it)
		ret = (*it).second->Update();

	for (auto it = std::begin(modules); it != std::end(modules) && ret == UPDATE_CONTINUE; ++it)
		ret = (*it).second->PostUpdate();

	float dt = (appTimer->Read() - ms) / 1000.0f;

	if (dt < 1000.0f / GetMaxFrameRate())
	{
		SDL_Delay((Uint32)(1000.0f / GetMaxFrameRate() - dt));
	}

	this->deltaTime = (appTimer->Read() - ms) / 1000.0f;

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (auto it = std::rbegin(modules); it != std::rend(modules) && ret == UPDATE_CONTINUE; ++it)
		ret = (*it).second->CleanUp();

	return ret;
}

ModuleWindow* Application::GetModuleWindow() const
{
	return FindModuleOfType<ModuleWindow>(ModuleType::WINDOW);
}

ModuleEditor* Application::GetModuleEditor() const
{
	return FindModuleOfType<ModuleEditor>(ModuleType::EDITOR);
}

ModuleInput* Application::GetModuleInput() const
{
	return FindModuleOfType<ModuleInput>(ModuleType::INPUT);
}

ModuleProgram* Application::GetModuleProgram() const
{
	return FindModuleOfType<ModuleProgram>(ModuleType::PROGRAM);
}

ModuleFileSystem* Application::GetModuleFileSystem() const
{
	return FindModuleOfType<ModuleFileSystem>(ModuleType::FILESYSTEM);
}

ModuleResources* Application::GetModuleResources() const
{
	return FindModuleOfType<ModuleResources>(ModuleType::RESOURCES);
}

ModuleEngineCamera* Application::GetModuleEngineCamera() const
{
	return FindModuleOfType<ModuleEngineCamera>(ModuleType::CAMERA);
}

ModuleScene* Application::GetModuleScene() const
{
	return FindModuleOfType<ModuleScene>(ModuleType::SCENE);
}

ModuleRender* Application::GetModuleRender() const
{
	return FindModuleOfType<ModuleRender>(ModuleType::RENDER);
}

ModuleDebugDraw* Application::GetModuleDebugDraw() const
{
	return FindModuleOfType<ModuleDebugDraw>(ModuleType::DEBUGDRAW);
}
