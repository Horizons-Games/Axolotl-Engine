#pragma once

#include "Enums/ModuleType_fwd.h"

// Order matters: they will Init/start/update in this order
enum class ModuleType
{
	WINDOW,
	FILESYSTEM,
	EDITOR,
	INPUT,
	PROGRAM,
	CAMERA,
	AUDIO,
	SCENE,
	PLAYER,
	RENDER,
	UI,
	RESOURCES,
	DEBUGDRAW,
	LAST = DEBUGDRAW,
};

template<>
struct IndexOfModule<ModuleRender>
{
	const static int value = static_cast<int>(ModuleType::RENDER);
};

template<>
struct IndexOfModule<ModuleWindow>
{
	const static int value = static_cast<int>(ModuleType::WINDOW);
};

template<>
struct IndexOfModule<ModuleInput>
{
	const static int value = static_cast<int>(ModuleType::INPUT);
};

template<>
struct IndexOfModule<ModuleProgram>
{
	const static int value = static_cast<int>(ModuleType::PROGRAM);
};

template<>
struct IndexOfModule<ModuleFileSystem>
{
	const static int value = static_cast<int>(ModuleType::FILESYSTEM);
};

template<>
struct IndexOfModule<ModuleResources>
{
	const static int value = static_cast<int>(ModuleType::RESOURCES);
};

template<>
struct IndexOfModule<ModuleScene>
{
	const static int value = static_cast<int>(ModuleType::SCENE);
};

template<>
struct IndexOfModule<ModuleCamera>
{
	const static int value = static_cast<int>(ModuleType::CAMERA);
};

template<>
struct IndexOfModule<ModuleDebugDraw>
{
	const static int value = static_cast<int>(ModuleType::DEBUGDRAW);
};

template<>
struct IndexOfModule<ModuleUI>
{
	const static int value = static_cast<int>(ModuleType::UI);
};

template<>
struct IndexOfModule<ModuleAudio>
{
	const static int value = static_cast<int>(ModuleType::AUDIO);
};

template<>
struct IndexOfModule<ModulePlayer>
{
	const static int value = static_cast<int>(ModuleType::PLAYER);
};

template<>
struct IndexOfModule<ModuleEditor>
{
	const static int value = static_cast<int>(ModuleType::EDITOR);
};
