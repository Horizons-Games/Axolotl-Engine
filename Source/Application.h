#pragma once

#include "Module.h"
#include "Timer/Timer.h"

#include <map>

class ModuleRender;
class ModuleWindow;
class ModuleInput;
class ModuleProgram;
class ModuleDebugDraw;
class ModuleEditor;
class ModuleEngineCamera;
class ModuleTexture;
class ModuleFileSystem;
class ModuleResources;
class ModuleScene;

enum class ModuleType
{
	WINDOW,
	EDITOR,
	INPUT,
	PROGRAM,
	FILESYSTEM,
	RESOURCES,
	CAMERA,
	SCENE,
	RENDER,
	DEBUGDRAW
};

class Application
{
public:

	Application();
	~Application();

	bool Start();
	bool Init();
	update_status Update();
	bool CleanUp();

	void SetMaxFrameRate(int maxFrames);
	int GetMaxFrameRate() const;
	float GetDeltaTime() const;

	ModuleWindow* GetModuleWindow() const;
	ModuleEditor* GetModuleEditor() const;
	ModuleInput* GetModuleInput() const;
	ModuleProgram* GetModuleProgram() const;
	ModuleFileSystem* GetModuleFileSystem() const;
	ModuleResources* GetModuleResources() const;
	ModuleEngineCamera* GetModuleEngineCamera() const;
	ModuleScene* GetModuleScene() const;
	ModuleRender* GetModuleRender() const;
	ModuleDebugDraw* GetModuleDebugDraw() const;

private:
	template <typename M,
		std::enable_if_t<std::is_base_of<Module, M>::value, bool> = true>
	M* FindModuleOfType(ModuleType type) const;

	std::map<ModuleType, std::unique_ptr<Module>> modules;
	std::unique_ptr<Timer> appTimer;

	int maxFramerate;
	float deltaTime = 0.f;
};

extern std::unique_ptr<Application> App;

inline void Application::SetMaxFrameRate(int maxFrames)
{
	maxFramerate = maxFrames;
}

inline int Application::GetMaxFrameRate() const
{
	return maxFramerate;
}

inline float Application::GetDeltaTime() const
{
	return deltaTime;
}

template<typename M,
	std::enable_if_t<std::is_base_of<Module, M>::value, bool>>
inline M* Application::FindModuleOfType(ModuleType type) const
{
	auto it = modules.find(type);
	if (it != std::end(modules))
	{
		return static_cast<M*>((*it).second.get());
	}
	return nullptr;
}
