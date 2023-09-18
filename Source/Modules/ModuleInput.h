#pragma once
#include "Math/float2.h"
#include "Module.h"
#include "SDL.h"
#include <map>

#define NUM_MOUSEBUTTONS 5
#define BMP_FREELOOKSURFACE "Assets/MouseCursors/freeLook.bmp"
#define BMP_ORBITSURFACE "Assets/MouseCursors/orbit.bmp"
#define BMP_MOVESURFACE "Assets/MouseCursors/move.bmp"
#define BMP_ZOOMSURFACE "Assets/MouseCursors/zoom.bmp"


namespace
{
enum class RumbleIntensity
{
	LOW,
	NORMAL,
	HIGH,
	HIGHEST
};
enum class RumbleDuration
{
	SHORT,
	NORMAL,
	LONG,
	LONGER,
};

const std::map<RumbleIntensity, Uint16> defaultRumbleIntensityMap({
	{ RumbleIntensity::LOW, 8192 },
	{ RumbleIntensity::NORMAL, 16384 },
	{ RumbleIntensity::HIGH, 24576 },
	{ RumbleIntensity::HIGHEST, 32767 },
});
const std::map<RumbleDuration, Uint16> defaultRumbleDurationMap({
	{ RumbleDuration::SHORT, 125 },
	{ RumbleDuration::NORMAL, 250 },
	{ RumbleDuration::LONG, 500 },
	{ RumbleDuration::LONGER, 1000 },
});
} // namespace

enum class KeyState
{
	IDLE,
	DOWN,
	UP,
	REPEAT
};

enum class JoystickHorizontalDirection
{
	LEFT,
	RIGHT,
	NONE
};

enum class JoystickVerticalDirection
{
	FORWARD,
	BACK,
	NONE
};

enum class InputMethod
{
	KEYBOARD,
	GAMEPAD
};

struct JoystickMovement
{
	JoystickHorizontalDirection horizontalMovement;
	JoystickVerticalDirection verticalMovement;
};

class ModuleInput : public Module
{
public:
	ModuleInput();
	~ModuleInput() override;

	bool Init() override;
	UpdateStatus Update() override;
	bool CleanUp() override;

	SDL_GameControllerAxis GetJoystickAxis() const;
	Sint16 GetJoystickAxisValue() const;

	KeyState GetKey(int scanCode) const;
	KeyState GetMouseButton(int mouseButton) const;
	KeyState GetGamepadButton(int gamepadButton) const;

	InputMethod GetCurrentInputMethod() const;
	
	// This setter methods will override user input
	// Use them with care
	void SetKey(SDL_Scancode scanCode, KeyState newState);
	void SetMouseButton(Uint8 mouseButtonCode, KeyState newState);

	SDL_GameController* FindController() const;
	SDL_JoystickID GetControllerInstanceID(SDL_GameController* controller) const;

	JoystickMovement GetDirection() const;

	void Rumble(RumbleIntensity intensityLeft, RumbleIntensity intensityRight, RumbleDuration durationMs) const;
	// Overload with same intensity on both sides
	void Rumble(RumbleIntensity intensity, RumbleDuration durationMs) const;
	// Overload for default Rumble
	void Rumble() const;

	float2 GetMouseMotion() const;
	float2 GetMouseWheel() const;
	float2 GetMousePosition() const;
	bool GetInFocus() const;

	void SetMousePositionX(int mouseX);
	void SetMousePositionY(int mouseY);

	void SetMouseMotionX(float posX);
	void SetMouseMotionY(float posY);
	void SetFreeLookCursor();
	void SetOrbitCursor();
	void SetMoveCursor();
	void SetZoomCursor();
	void SetDefaultCursor();
	void SetShowCursor(bool set);

	bool IsMouseWheelScrolled() const;

	KeyState operator[](SDL_Scancode index);

private:
	KeyState keysState[SDL_NUM_SCANCODES] = { KeyState::IDLE };
	KeyState mouseButtonState[NUM_MOUSEBUTTONS] = { KeyState::IDLE };
	KeyState gamepadState[SDL_CONTROLLER_BUTTON_MAX] = { KeyState::IDLE };
	
	const std::map<RumbleIntensity, Uint16> rumbleIntensityMap;
	const std::map<RumbleDuration, Uint16> rumbleDurationMap;
	
	float2 mouseWheel;
	float2 mouseMotion;

	int mousePosX;
	int mousePosY;

	JoystickMovement direction;
	InputMethod inputMethod;

	bool mouseWheelScrolled;
	bool inFocus;

	SDL_GameControllerAxis axis;
	Sint16 axisValue;

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
	std::unique_ptr<SDL_Cursor, SDLCursorDestroyer> defaultCursor;
};

inline SDL_JoystickID ModuleInput::GetControllerInstanceID(SDL_GameController* controller) const
{
	return SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
}

inline SDL_GameController* ModuleInput::FindController() const
{
	for (int i = 0; i < SDL_NumJoysticks(); i++)
	{
		if (SDL_IsGameController(i))
		{
			return SDL_GameControllerOpen(i);
		}
	}

	return nullptr;
}

inline KeyState ModuleInput::GetKey(int scanCode) const
{
	return keysState[scanCode];
}

inline KeyState ModuleInput::GetMouseButton(int mouseButton) const
{
	return mouseButtonState[mouseButton];
}

inline KeyState ModuleInput::GetGamepadButton(int gamepadButton) const
{
	return gamepadState[gamepadButton];
}

inline void ModuleInput::SetKey(SDL_Scancode scanCode, KeyState newState)
{
	keysState[scanCode] = newState;
}

inline void ModuleInput::SetMouseButton(Uint8 mouseButtonCode, KeyState newState)
{
	mouseButtonState[mouseButtonCode] = newState;
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
	return float2((float) mousePosX, (float) mousePosY);
}

inline bool ModuleInput::GetInFocus() const
{
	return inFocus;
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
	mouseMotion.x = posX;
}

inline void ModuleInput::SetMouseMotionY(float posY)
{
	mouseMotion.y = posY;
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

inline void ModuleInput::SetDefaultCursor()
{
	SDL_SetCursor(defaultCursor.get());
}

inline void ModuleInput::SetShowCursor(bool set)
{
	set ? SDL_ShowCursor(SDL_ENABLE) : SDL_ShowCursor(SDL_DISABLE);
}

inline bool ModuleInput::IsMouseWheelScrolled() const
{
	return mouseWheelScrolled;
}

inline KeyState ModuleInput::operator[](SDL_Scancode index)
{
	return keysState[index];
}

inline JoystickMovement ModuleInput::GetDirection() const
{
	return direction;
}

inline SDL_GameControllerAxis ModuleInput::GetJoystickAxis() const
{
	return axis;
}

inline Sint16 ModuleInput::GetJoystickAxisValue() const
{
	return axisValue;
}

inline InputMethod ModuleInput::GetCurrentInputMethod() const
{
	return inputMethod;
}
