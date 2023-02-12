#pragma once
#include "Module.h"
#include "Math/float2.h"

#define NUM_MOUSEBUTTONS 5
#define BMP_FREELOOKSURFACE "Assets/MouseCursors/freeLook.bmp"
#define BMP_ORBITSURFACE "Assets/MouseCursors/orbit.bmp"
#define BMP_MOVESURFACE "Assets/MouseCursors/move.bmp"
#define BMP_ZOOMSURFACE "Assets/MouseCursors/zoom.bmp"

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

	void SetMouseMotionX(float posX);
	void SetMouseMotionY(float posY);
	void SetFreeLookCursor();
	void SetOrbitCursor();
	void SetMoveCursor();
	void SetZoomCursor();

	bool IsMouseWheelScrolled() const;

private:
	const uint8_t* keyboard = NULL;

	KeyState keysState[SDL_NUM_SCANCODES] = { KeyState::IDLE };
	KeyState mouseButtonState[NUM_MOUSEBUTTONS] = { KeyState::IDLE };

	float2 mouseWheel = float2::zero;
	float2 mouseMotion = float2::zero;

	int mousePosX = 0;
	int mousePosY = 0;

	bool mouseWheelScrolled;

	SDL_Surface* freeLookSurface;
	SDL_Surface* orbitSurface;
	SDL_Surface* moveSurface;
	SDL_Surface* zoomSurface;
	SDL_Cursor* freeLookCursor;
	SDL_Cursor* orbitCursor;
	SDL_Cursor* moveCursor;
	SDL_Cursor* zoomCursor;
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

inline void ModuleInput::SetMouseMotionX(float posX)
{
	this->mouseMotion.first = posX;
}

inline void ModuleInput::SetMouseMotionY(float posY)
{
	this->mouseMotion.second = posY;
}

inline void ModuleInput::SetFreeLookCursor()
{
	SDL_SetCursor(freeLookCursor);
}

inline void ModuleInput::SetOrbitCursor()
{
	SDL_SetCursor(orbitCursor);
}

inline void ModuleInput::SetMoveCursor()
{
	SDL_SetCursor(moveCursor);
}

inline void ModuleInput::SetZoomCursor()
{
	SDL_SetCursor(zoomCursor);
}

inline bool ModuleInput::IsMouseWheelScrolled() const
{
	return mouseWheelScrolled;
}
