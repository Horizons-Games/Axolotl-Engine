#pragma once

class ModuleRender;
class ModuleWindow;
class ModuleInput;
class ModuleProgram;
class ModuleTexture;
class ModuleFileSystem;
class ModuleResources;
class ModuleScene;
class ModuleCamera;
class ModuleDebugDraw;
class ModuleUI;
class ModuleAudio;
class ModulePlayer;
class ModuleEditor;

enum class ModuleType
{
	RENDER,
	WINDOW,
	INPUT,
	PROGRAM,
	TEXTURE,
	FILESYSTEM,
	RESOURCES,
	SCENE,
	CAMERA,
	DEBUGDRAW,
	UI,
	AUDIO,
	PLAYER,
	EDITOR
};

template<typename T>
struct ModuleToEnum
{
};

template<>
struct ModuleToEnum<ModuleRender>
{
	const static ModuleType value = ModuleType::RENDER;
};

template<>
struct ModuleToEnum<ModuleWindow>
{
	const static ModuleType value = ModuleType::WINDOW;
};

template<>
struct ModuleToEnum<ModuleInput>
{
	const static ModuleType value = ModuleType::INPUT;
};

template<>
struct ModuleToEnum<ModuleProgram>
{
	const static ModuleType value = ModuleType::PROGRAM;
};

template<>
struct ModuleToEnum<ModuleTexture>
{
	const static ModuleType value = ModuleType::TEXTURE;
};

template<>
struct ModuleToEnum<ModuleFileSystem>
{
	const static ModuleType value = ModuleType::FILESYSTEM;
};

template<>
struct ModuleToEnum<ModuleResources>
{
	const static ModuleType value = ModuleType::RESOURCES;
};

template<>
struct ModuleToEnum<ModuleScene>
{
	const static ModuleType value = ModuleType::SCENE;
};

template<>
struct ModuleToEnum<ModuleCamera>
{
	const static ModuleType value = ModuleType::CAMERA;
};

template<>
struct ModuleToEnum<ModuleDebugDraw>
{
	const static ModuleType value = ModuleType::DEBUGDRAW;
};

template<>
struct ModuleToEnum<ModuleUI>
{
	const static ModuleType value = ModuleType::UI;
};

template<>
struct ModuleToEnum<ModuleAudio>
{
	const static ModuleType value = ModuleType::AUDIO;
};

template<>
struct ModuleToEnum<ModulePlayer>
{
	const static ModuleType value = ModuleType::PLAYER;
};

template<>
struct ModuleToEnum<ModuleEditor>
{
	const static ModuleType value = ModuleType::EDITOR;
};
