#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleUI.h"
#include "Scene/Scene.h"
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
	bool ret = true;
	SDL_Init(0);

	if(SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
        ENGINE_LOG("SDL_EVENTS could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
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

	return ret;
}

update_status ModuleInput::Update()
{
#ifdef DEBUG
    OPTICK_CATEGORY("UpdateInput", Optick::Category::Input);
#endif // DEBUG

    update_status status = update_status::UPDATE_CONTINUE;

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

    const Uint8* keyboard = SDL_GetKeyboardState(NULL);

    if (keyboard[SDL_SCANCODE_ESCAPE]) 
    {
        if (App->GetIsOnPlayMode())
        {
            App->OnStopPlay();
        }
        else
        {
            status = update_status::UPDATE_STOP;
        }
    }

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
                App->renderer->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
                App->renderer->UpdateBuffers(sdlEvent.window.data1, sdlEvent.window.data2);
                App->userInterface->RecalculateCanvasSizeAndScreenFactor();
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
            char* droppedFilePath = sdlEvent.drop.file;

            std::string dropFilePath(droppedFilePath);
            std::replace(dropFilePath.begin(), dropFilePath.end(), '\\', '/'); 
            App->scene->GetLoadedScene()->ConvertModelIntoGameObject(droppedFilePath);
            SDL_free(droppedFilePath);    // Free dropped_filedir memory
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

    return status;
}

void ModuleInput::SetShowCursor(bool set)
{
    set ? SDL_ShowCursor(SDL_ENABLE) : SDL_ShowCursor(SDL_DISABLE);
}

bool ModuleInput::CleanUp()
{
    ENGINE_LOG("Quitting SDL input event subsystem");

	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	
    return true;
}
