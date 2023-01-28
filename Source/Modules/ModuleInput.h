#pragma once
#include "Module.h"

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

	float GetMouseMotionX() const;
	float GetMouseMotionY() const;
	float GetMouseWheelX() const;
	float GetMouseWheelY() const;

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

	std::pair<float, float> mouseWheel;
	std::pair<float, float> mouseMotion;

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
	return this->mouseWheelScrolled;
}
