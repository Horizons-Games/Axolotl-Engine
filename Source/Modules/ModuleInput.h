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
	KeyState keysState[SDL_NUM_SCANCODES] = { KeyState::IDLE };
	KeyState mouseButtonState[NUM_MOUSEBUTTONS] = { KeyState::IDLE };

	float2 mouseWheel = float2::zero;
	float2 mouseMotion = float2::zero;

	int mousePosX = 0;
	int mousePosY = 0;

	bool mouseWheelScrolled;

	struct SDLSurfaceDestroyer
	{
		void operator()(SDL_Surface* surface) const
		{
			SDL_FreeSurface(surface);
		}
	};

	struct SDLCursorDestroyer
	{
		void operator()(SDL_Cursor* cursor) const
		{
			SDL_FreeCursor(cursor);
		}
	};

	std::unique_ptr<SDL_Surface, SDLSurfaceDestroyer> freeLookSurface;
	std::unique_ptr<SDL_Surface, SDLSurfaceDestroyer> orbitSurface;
	std::unique_ptr<SDL_Surface, SDLSurfaceDestroyer> moveSurface;
	std::unique_ptr<SDL_Surface, SDLSurfaceDestroyer> zoomSurface;
	std::unique_ptr<SDL_Cursor, SDLCursorDestroyer> freeLookCursor;
	std::unique_ptr<SDL_Cursor, SDLCursorDestroyer> orbitCursor;
	std::unique_ptr<SDL_Cursor, SDLCursorDestroyer> moveCursor;
	std::unique_ptr<SDL_Cursor, SDLCursorDestroyer> zoomCursor;
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
	this->mouseMotion.x = posX;
}

inline void ModuleInput::SetMouseMotionY(float posY)
{
	this->mouseMotion.y = posY;
}

inline void ModuleInput::SetFreeLookCursor()
{
	SDL_SetCursor(freeLookCursor.get());
}

inline void ModuleInput::SetOrbitCursor()
{
	SDL_SetCursor(orbitCursor.get());
}

inline void ModuleInput::SetMoveCursor()
{
	SDL_SetCursor(moveCursor.get());
}

inline void ModuleInput::SetZoomCursor()
{
	SDL_SetCursor(zoomCursor.get());
}

inline bool ModuleInput::IsMouseWheelScrolled() const
{
	return mouseWheelScrolled;
}
