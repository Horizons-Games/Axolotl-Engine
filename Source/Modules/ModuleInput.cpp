#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePlayer.h"
#include "ModuleCamera.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "Scene/Scene.h"
#include "Windows/WindowMainMenu.h"
#include "ModuleUI.h"
#include "imgui_impl_sdl.h"

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
    ENGINE_LOG("Init SDL input event system");
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
        ENGINE_LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		return false;
	}

#ifdef ENGINE
        freeLookSurface = std::unique_ptr<SDL_Surface, SDLSurfaceDestroyer>(SDL_LoadBMP(BMP_FREELOOKSURFACE));
        orbitSurface = std::unique_ptr<SDL_Surface, SDLSurfaceDestroyer>(SDL_LoadBMP(BMP_ORBITSURFACE));
        moveSurface = std::unique_ptr<SDL_Surface, SDLSurfaceDestroyer>(SDL_LoadBMP(BMP_MOVESURFACE));
        zoomSurface = std::unique_ptr<SDL_Surface, SDLSurfaceDestroyer>(SDL_LoadBMP(BMP_ZOOMSURFACE));
        freeLookCursor = std::unique_ptr<SDL_Cursor, SDLCursorDestroyer>
                        (SDL_CreateColorCursor(freeLookSurface.get(), 0, 0));
        orbitCursor = std::unique_ptr<SDL_Cursor, SDLCursorDestroyer>
                        (SDL_CreateColorCursor(orbitSurface.get(), 0, 0));
        moveCursor = std::unique_ptr<SDL_Cursor, SDLCursorDestroyer>
                        (SDL_CreateColorCursor(moveSurface.get(), 0, 0));
        zoomCursor = std::unique_ptr<SDL_Cursor, SDLCursorDestroyer>
                        (SDL_CreateColorCursor(zoomSurface.get(), 0, 0));

        std::unique_ptr<SDL_Cursor, SDLCursorDestroyer> defaultCursor = 
                        std::unique_ptr<SDL_Cursor, SDLCursorDestroyer>(SDL_GetCursor());
        this->defaultCursor = std::unique_ptr<SDL_Cursor, SDLCursorDestroyer>
                        (defaultCursor.get());
#else  // ENGINE
    SetShowCursor(false);
#endif // GAMEMODE

	return true;
}

update_status ModuleInput::Update()
{
#ifdef DEBUG
    OPTICK_CATEGORY("UpdateInput", Optick::Category::Input);
#endif // DEBUG

    mouseMotion = float2::zero;
    mouseWheelScrolled = false;
    
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        if (keysState[i] == KeyState::DOWN)
        {
            keysState[i] = KeyState::REPEAT;
        }

        if (keysState[i] == KeyState::UP)
        {
            keysState[i] = KeyState::IDLE;
        }
    }

    for (int i = 0; i < NUM_MOUSEBUTTONS; ++i) {
        if (mouseButtonState[i] == KeyState::DOWN)
        {
            mouseButtonState[i] = KeyState::REPEAT;
        }

        if (mouseButtonState[i] == KeyState::UP)
        {
            mouseButtonState[i] = KeyState::IDLE;
        }
    }

    SDL_PumpEvents();

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
            return update_status::UPDATE_STOP;
        
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
            keysState[sdlEvent.key.keysym.scancode] = KeyState::DOWN;
            break;
        case SDL_KEYUP:
            keysState[sdlEvent.key.keysym.scancode] = KeyState::UP;
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
            mouseMotion = float2((float)sdlEvent.motion.xrel, (float)sdlEvent.motion.yrel);
            break;

        case SDL_MOUSEWHEEL:
            mouseWheel = float2((float)sdlEvent.wheel.x, (float)sdlEvent.wheel.y);
            mouseWheelScrolled = true;
            break;

        case SDL_DROPFILE:
            char* droppedFilePathAsChar = sdlEvent.drop.file;

            std::string droppedFilePathString(droppedFilePathAsChar);
            std::replace(droppedFilePathString.begin(), droppedFilePathString.end(), '\\', '/'); 
            App->GetModule<ModuleScene>()->GetLoadedScene()->ConvertModelIntoGameObject(droppedFilePathString);
            SDL_free(droppedFilePathAsChar);    // Free dropped_filedir memory
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
    if ((keysState[SDL_SCANCODE_LCTRL] == KeyState::REPEAT 
        || keysState[SDL_SCANCODE_LCTRL] == KeyState::DOWN)
        && keysState[SDL_SCANCODE_Q] == KeyState::DOWN)
    {
        if (App->IsOnPlayMode())
        {
            App->OnStop();
        }
    }

    if ((keysState[SDL_SCANCODE_LCTRL] == KeyState::REPEAT
        || keysState[SDL_SCANCODE_LCTRL] == KeyState::DOWN)
        && keysState[SDL_SCANCODE_A] == KeyState::DOWN)
    {
        if (App->IsOnPlayMode())
        {
            SDL_ShowCursor(SDL_QUERY) ? SetShowCursor(false) : SetShowCursor(true);
        }
    }

    if (keysState[SDL_SCANCODE_LCTRL] == KeyState::REPEAT 
        && keysState[SDL_SCANCODE_S] == KeyState::DOWN
        && SDL_ShowCursor(SDL_QUERY))
    {
        App->GetModule<ModuleEditor>()->GetMainMenu()->ShortcutSave();
    }
#endif

    return update_status::UPDATE_CONTINUE;
}

bool ModuleInput::CleanUp()
{
    ENGINE_LOG("Quitting SDL input event subsystem");

	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	
    return true;
}
