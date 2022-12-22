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
	~ModuleInput() override;

	bool Init();
	update_status Update();
	bool CleanUp();

	inline KeyState GetKey(int scanCode) const { return this->keysState[scanCode]; }
	inline KeyState GetMouseButton(int mouseButton) const { return this->mouseButtonState[mouseButton]; }

	inline float GetMouseMotionX() const { return this->mouseMotion.first; }
	inline float GetMouseMotionY() const { return this->mouseMotion.second; }
	inline float GetMouseWheelX() const { return this->mouseWheel.first; }
	inline float GetMouseWheelY() const { return this->mouseWheel.second; }

	inline bool IsMouseWeelScrolled() const { return this->mouseWheelScrolled; }

private:
	const Uint8 *keyboard = NULL;

	KeyState keysState[SDL_NUM_SCANCODES] = { KeyState::IDLE };
	KeyState mouseButtonState[NUM_MOUSEBUTTONS] = { KeyState::IDLE };

	std::pair<float, float> mouseWheel;
	std::pair<float, float> mouseMotion;

	bool mouseWheelScrolled;
};