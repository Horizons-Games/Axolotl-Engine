#pragma once
#include "Module.h"

#define NUM_MOUSEBUTTONS 5

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

	KeyState GetKey(int scanCode) const;
	KeyState GetMouseButton(int mouseButton) const;

	float GetMouseMotionX() const;
	float GetMouseMotionY() const;
	float GetMouseWheelX() const;
	float GetMouseWheelY() const;

	void SetMouseMotionX(float posX);
	void SetMouseMotionY(float posY);

	bool IsMouseWheelScrolled() const;

private:
	const uint8_t* keyboard = NULL;

	KeyState keysState[SDL_NUM_SCANCODES] = { KeyState::IDLE };
	KeyState mouseButtonState[NUM_MOUSEBUTTONS] = { KeyState::IDLE };

	std::pair<float, float> mouseWheel;
	std::pair<float, float> mouseMotion;

	bool mouseWheelScrolled;
};

inline KeyState ModuleInput::GetKey(int scanCode) const
{
	return this->keysState[scanCode];
}

inline KeyState ModuleInput::GetMouseButton(int mouseButton) const
{
	return this->mouseButtonState[mouseButton];
}

inline float ModuleInput::GetMouseMotionX() const
{
	return this->mouseMotion.first; 
}

inline float ModuleInput::GetMouseMotionY() const
{ 
	return this->mouseMotion.second;
}

inline float ModuleInput::GetMouseWheelX() const
{
	return this->mouseWheel.first;
}

inline float ModuleInput::GetMouseWheelY() const 
{
	return this->mouseWheel.second;
}

inline void ModuleInput::SetMouseMotionX(float posX)
{
	this->mouseMotion.first = posX;
}

inline void ModuleInput::SetMouseMotionY(float posY)
{
	this->mouseMotion.second = posY;
}

inline bool ModuleInput::IsMouseWheelScrolled() const
{
	return this->mouseWheelScrolled;
}
