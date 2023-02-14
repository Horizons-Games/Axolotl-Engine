#pragma once
#include "Module.h"
#include "Math/float2.h"

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
	
	float2 GetMouseMotion() const;
	float2 GetMouseWheel() const;
	float2 GetMousePosition() const;

	void SetMousePositionX(int mouseX);
	void SetMousePositionY(int mouseY);

	bool IsMouseWheelScrolled() const;

private:
	KeyState keysState[SDL_NUM_SCANCODES] = { KeyState::IDLE };
	KeyState mouseButtonState[NUM_MOUSEBUTTONS] = { KeyState::IDLE };

	float2 mouseWheel = float2::zero;
	float2 mouseMotion = float2::zero;

	int mousePosX = 0;
	int mousePosY = 0;

	bool mouseWheelScrolled;
};

inline KeyState ModuleInput::GetKey(int scanCode) const
{
	return keysState[scanCode];
}

inline KeyState ModuleInput::GetMouseButton(int mouseButton) const
{
	return mouseButtonState[mouseButton];
}

inline float2 ModuleInput::GetMouseMotion() const
{
	return mouseMotion; 
}

inline float2 ModuleInput::GetMouseWheel() const
{
	return mouseWheel;
}

inline float2 ModuleInput::GetMousePosition() const
{
	return float2((float)mousePosX, (float)mousePosY);
}

inline void ModuleInput::SetMousePositionX(int mouseX)
{
	mousePosX = mouseX;
}

inline void ModuleInput::SetMousePositionY(int mouseY)
{
	mousePosY = mouseY;
}

inline bool ModuleInput::IsMouseWheelScrolled() const
{
	return mouseWheelScrolled;
}
