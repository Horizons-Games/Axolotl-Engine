#include "StdAfx.h"

#include "Modules/ModuleInput.h"

#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleEditor.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleUI.h"
#include "Scene/Scene.h"
#include "Windows/WindowMainMenu.h"
#include "imgui_impl_sdl.h"
#include "AxoLog.h"

#ifdef DEBUG
	#include "optick.h"
#endif // DEBUG

ModuleInput::ModuleInput() : mouseWheel(float2::zero), mouseMotion(float2::zero), mousePosX(0), mousePosY(0)
{
}

ModuleInput::~ModuleInput()
{
}

bool ModuleInput::Init()
{
	LOG_VERBOSE("Init SDL input event system");
	SDL_Init(0);

	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LOG_ERROR("SDL_EVENTS could not initialize! SDL_Error: {}\n", SDL_GetError());
		return false;
	}

#ifdef ENGINE
	freeLookSurface = std::unique_ptr<SDL_Surface, SDLSurfaceDestroyer>(SDL_LoadBMP(BMP_FREELOOKSURFACE));
	orbitSurface = std::unique_ptr<SDL_Surface, SDLSurfaceDestroyer>(SDL_LoadBMP(BMP_ORBITSURFACE));
	moveSurface = std::unique_ptr<SDL_Surface, SDLSurfaceDestroyer>(SDL_LoadBMP(BMP_MOVESURFACE));
	zoomSurface = std::unique_ptr<SDL_Surface, SDLSurfaceDestroyer>(SDL_LoadBMP(BMP_ZOOMSURFACE));
	freeLookCursor =
		std::unique_ptr<SDL_Cursor, SDLCursorDestroyer>(SDL_CreateColorCursor(freeLookSurface.get(), 0, 0));
	orbitCursor = std::unique_ptr<SDL_Cursor, SDLCursorDestroyer>(SDL_CreateColorCursor(orbitSurface.get(), 0, 0));
	moveCursor = std::unique_ptr<SDL_Cursor, SDLCursorDestroyer>(SDL_CreateColorCursor(moveSurface.get(), 0, 0));
	zoomCursor = std::unique_ptr<SDL_Cursor, SDLCursorDestroyer>(SDL_CreateColorCursor(zoomSurface.get(), 0, 0));

	std::unique_ptr<SDL_Cursor, SDLCursorDestroyer> defaultCursor =
		std::unique_ptr<SDL_Cursor, SDLCursorDestroyer>(SDL_GetCursor());
	this->defaultCursor = std::unique_ptr<SDL_Cursor, SDLCursorDestroyer>(defaultCursor.get());
#else  // ENGINE
	SetShowCursor(false);
#endif // GAMEMODE

	return true;
}

UpdateStatus ModuleInput::Update()
{
#ifdef DEBUG
	OPTICK_CATEGORY("UpdateInput", Optick::Category::Input);
#endif // DEBUG

	mouseMotion = float2::zero;
	mouseWheelScrolled = false;

	for (int i = 0; i < SDL_NUM_SCANCODES; ++i)
	{
		if (keysState[i] == KeyState::DOWN)
		{
			keysState[i] = KeyState::REPEAT;
		}

		if (keysState[i] == KeyState::UP)
		{
			keysState[i] = KeyState::IDLE;
		}
	}

	for (int i = 0; i < NUM_MOUSEBUTTONS; ++i)
	{
		if (mouseButtonState[i] == KeyState::DOWN)
		{
			mouseButtonState[i] = KeyState::REPEAT;
		}

		if (mouseButtonState[i] == KeyState::UP)
		{
			mouseButtonState[i] = KeyState::IDLE;
		}
	}

	for (int i = 0; i < SDL_CONTROLLER_BUTTON_MAX; ++i)
	{
		if (gamepadState[i] == KeyState::DOWN)
		{
			gamepadState[i] = KeyState::REPEAT;
		}

		if (gamepadState[i] == KeyState::UP)
		{
			gamepadState[i] = KeyState::IDLE;
		}
	}

	SDL_PumpEvents();

	SDL_GameController* controller = FindController();

	SDL_Event sdlEvent;

	while (SDL_PollEvent(&sdlEvent) != 0)
	{
#ifdef ENGINE
		ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
#else // ENGINE

		if (App->IsDebuggingGame())
		{
			ImGui_ImplSDL2_ProcessEvent(&sdlEvent);
		}
#endif
	

		switch (sdlEvent.type)
		{
			case SDL_QUIT:
				return UpdateStatus::UPDATE_STOP;

			case SDL_WINDOWEVENT:
				if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED ||
					sdlEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					ModuleRender* render = App->GetModule<ModuleRender>();
					render->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
					render->UpdateBuffers(sdlEvent.window.data1, sdlEvent.window.data2);
					App->GetModule<ModuleUI>()->RecalculateCanvasSizeAndScreenFactor();
					App->GetModule<ModuleCamera>()->RecalculateOrthoProjectionMatrix();
				}
				if (sdlEvent.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
				{
					inFocus = false;
				}
				if (sdlEvent.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
				{
					inFocus = true;
				}
				break;

			case SDL_KEYDOWN:
				if (sdlEvent.key.repeat == 0)
				{
					keysState[sdlEvent.key.keysym.scancode] = KeyState::DOWN;
				}
				break;

			case SDL_KEYUP:
				if (sdlEvent.key.repeat == 0)
				{
					keysState[sdlEvent.key.keysym.scancode] = KeyState::UP;
				}
				break;

			case SDL_MOUSEBUTTONDOWN:
				mouseButtonState[sdlEvent.button.button] = KeyState::DOWN;
				break;

			case SDL_MOUSEBUTTONUP:
				mouseButtonState[sdlEvent.button.button] = KeyState::UP;
				break;

			case SDL_MOUSEMOTION:
				mousePosX = sdlEvent.motion.x;
				mousePosY = sdlEvent.motion.y;
				mouseMotion = float2((float) sdlEvent.motion.xrel, (float) sdlEvent.motion.yrel);
				break;

			case SDL_MOUSEWHEEL:
				mouseWheel = float2((float) sdlEvent.wheel.x, (float) sdlEvent.wheel.y);
				mouseWheelScrolled = true;
				break;

			case SDL_CONTROLLERDEVICEADDED:
				if (!controller)
				{
					controller = SDL_GameControllerOpen(sdlEvent.cdevice.which);
				}
				break;

			case SDL_CONTROLLERDEVICEREMOVED:
				if (controller && sdlEvent.cdevice.which == GetControllerInstanceID(controller))
				{
					SDL_GameControllerClose(controller);
					controller = FindController();
				}
				break;

			case SDL_CONTROLLERBUTTONDOWN:
				if (controller && sdlEvent.cdevice.which == GetControllerInstanceID(controller))
				{
					gamepadState[sdlEvent.cbutton.button] = KeyState::DOWN;
				}
				break;
					/*switch (sdlEvent.cbutton.button)
					{
						case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_A:
							LOG_DEBUG("A PRESSED");
							break;
						case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_B:
							LOG_DEBUG("B PRESSED");
							break;
						case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_X:
							LOG_DEBUG("X PRESSED");
							break;
						case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_Y:
							LOG_DEBUG("Y PRESSED");
							break;
						case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_BACK:
							LOG_DEBUG("BACK PRESSED");
							break;
						case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_DOWN:
							LOG_DEBUG("DOWN PRESSED");
							break;
						case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_LEFT:
							LOG_DEBUG("LEFT PRESSED");
							break;
						case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
							LOG_DEBUG("RIGHT PRESSED");
							break;
						case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_DPAD_UP:
							LOG_DEBUG("UP PRESSED");
							break;
						case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSTICK:
							LOG_DEBUG("RIGHT STICK PRESSED");
							break;
						case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
							LOG_DEBUG("RIGHT SHOULDER PRESSED");
							break;
						case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSTICK:
							LOG_DEBUG("LEFT STICK PRESSED");
							break;
						case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
							LOG_DEBUG("LEFT SHOULDER PRESSED");
							break;
						case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_GUIDE:
							LOG_DEBUG("GUIDE PRESSED");
							break;
						case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_START:
							LOG_DEBUG("START PRESSED");
							break;
						case SDL_GameControllerButton::SDL_CONTROLLER_BUTTON_TOUCHPAD:
							LOG_DEBUG("TOUCHPAD PRESSED");
							break;
					}
				break;*/

			case SDL_CONTROLLERBUTTONUP:
				if (controller && sdlEvent.cdevice.which == GetControllerInstanceID(controller))
				{
					gamepadState[sdlEvent.cbutton.button] = KeyState::UP;
				}
				break;

			case SDL_JOYAXISMOTION: 
				if (controller)
				{
					int axis = sdlEvent.jaxis.axis;
					Sint16 axisValue = sdlEvent.jaxis.value;
					switch (axis)
					{
						case 0:
							if (axisValue < -3200) 
							{
								LOG_DEBUG("JOYSTICK LEFT");
							}
							else if (axisValue > 3200)
							{
								LOG_DEBUG("JOYSTICK RIGHT");
							}
							else
							{
							}
							break;

						case 1:
							if (axisValue < -3200)
							{
								LOG_DEBUG("JOYSTICK DOWN");
							}
							else if (axisValue > 3200)
							{
								LOG_DEBUG("JOYSTICK UP");
							}
							else
							{
							}
							break;

						default:
							break;
					}
				}
				break;

			case SDL_DROPFILE:
				char* droppedFilePathAsChar = sdlEvent.drop.file;

				std::string droppedFilePathString(droppedFilePathAsChar);
				std::replace(droppedFilePathString.begin(), droppedFilePathString.end(), '\\', '/');
				App->GetModule<ModuleScene>()->GetLoadedScene()->ConvertModelIntoGameObject(droppedFilePathString);
				SDL_free(droppedFilePathAsChar); // Free dropped_filedir memory
				break;
		}
	}

	if (keysState[SDL_SCANCODE_LALT] == KeyState::REPEAT && keysState[SDL_SCANCODE_J] == KeyState::DOWN)
	{
		App->SwitchDebuggingGame();
#ifndef ENGINE
		if (!App->IsDebuggingGame())
		{
			SetShowCursor(false);
		}
#endif // ENGINE
	}

#ifdef ENGINE
	if ((keysState[SDL_SCANCODE_LCTRL] == KeyState::REPEAT || keysState[SDL_SCANCODE_LCTRL] == KeyState::DOWN) &&
		keysState[SDL_SCANCODE_Q] == KeyState::DOWN)
	{
		if (App->IsOnPlayMode())
		{
			App->OnStop();
		}
	}

	if ((keysState[SDL_SCANCODE_LCTRL] == KeyState::REPEAT || keysState[SDL_SCANCODE_LCTRL] == KeyState::DOWN) &&
		keysState[SDL_SCANCODE_A] == KeyState::DOWN)
	{
		if (App->IsOnPlayMode())
		{
			SDL_ShowCursor(SDL_QUERY) ? SetShowCursor(false) : SetShowCursor(true);
		}
	}

	if (keysState[SDL_SCANCODE_LCTRL] == KeyState::REPEAT && keysState[SDL_SCANCODE_S] == KeyState::DOWN &&
		SDL_ShowCursor(SDL_QUERY))
	{
		App->GetModule<ModuleEditor>()->GetMainMenu()->ShortcutSave();
	}

	if (keysState[SDL_SCANCODE_F5] == KeyState::DOWN && SDL_ShowCursor(SDL_QUERY))
	{
		App->GetModule<ModuleRender>()->ChangeRenderMode();
	}
#endif

	return UpdateStatus::UPDATE_CONTINUE;
}

bool ModuleInput::CleanUp()
{
	LOG_VERBOSE("Quitting SDL input event subsystem");

	SDL_QuitSubSystem(SDL_INIT_EVENTS);

	return true;
}
