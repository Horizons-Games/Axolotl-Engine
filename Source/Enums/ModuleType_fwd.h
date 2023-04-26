#pragma once

class ModuleRender;
class ModuleWindow;
class ModuleInput;
class ModuleProgram;
class ModuleFileSystem;
class ModuleResources;
class ModuleScene;
class ModuleCamera;
class ModuleDebugDraw;
class ModuleUI;
class ModuleAudio;
class ModulePlayer;
class ModuleEditor;

enum class ModuleType;

template<typename T>
struct ModuleToEnum
{
	const static ModuleType value = static_cast<ModuleType>(-1);
};
