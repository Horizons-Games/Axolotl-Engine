#pragma once
#include "Module.h"
#include "Globals.h"

#include "SDL/include/SDL.h"

#define NUM_MOUSEBUTTONS 5

typedef unsigned __int8 Uint8;

enum class KeyState {
	IDLE,
	DOWN,
	UP,
	REPEAT
};

class ModuleInput : public Module
{
public:
	ModuleInput();
	~ModuleInput();

	bool Init();
	update_status Update();
	bool CleanUp();

	KeyState GetKey(int scanCode) const;
	KeyState GetMouseButton(int mouseButton) const;

	float GetMouseMotionX() const;
	float GetMouseMotionY() const;
	float GetMouseWheelX() const;
	float GetMouseWheelY() const; 

	bool IsMouseWeelScrolled() const;

private:
	const Uint8 *keyboard = NULL;

	KeyState keysState[SDL_NUM_SCANCODES] = { KeyState::IDLE };
	KeyState mouseButtonState[NUM_MOUSEBUTTONS] = { KeyState::IDLE };

	std::pair<float, float> mouseWheel;
	std::pair<float, float> mouseMotion;

	bool mouseWheelScrolled;
};