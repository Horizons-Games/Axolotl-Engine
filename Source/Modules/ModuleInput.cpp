#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "imgui_impl_sdl.h"

ModuleInput::ModuleInput()
{}

ModuleInput::~ModuleInput()
{}

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

	return ret;
}

update_status ModuleInput::Update()
{
    update_status status = UPDATE_CONTINUE;

    mouseMotion = float2::zero;
    mouseWheelScrolled = false;
    
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        if (keysState[i] == KeyState::DOWN)
            keysState[i] = KeyState::REPEAT;

        if (keysState[i] == KeyState::UP)
            keysState[i] = KeyState::IDLE;
    }

    for (int i = 0; i < NUM_MOUSEBUTTONS; ++i) {
        if (mouseButtonState[i] == KeyState::DOWN)
            mouseButtonState[i] = KeyState::REPEAT;

        if (mouseButtonState[i] == KeyState::UP)
            mouseButtonState[i] = KeyState::IDLE;
    }

    SDL_PumpEvents();

    const Uint8* keyboard = SDL_GetKeyboardState(NULL);

    if (keyboard[SDL_SCANCODE_ESCAPE]) 
    {
        status = UPDATE_STOP;
    }

    SDL_Event sdlEvent;

    while (SDL_PollEvent(&sdlEvent) != 0)
    {
        ImGui_ImplSDL2_ProcessEvent(&sdlEvent);

        switch (sdlEvent.type)
        {
        case SDL_QUIT:
            return UPDATE_STOP;
        
        case SDL_WINDOWEVENT:
            if (sdlEvent.window.event == SDL_WINDOWEVENT_RESIZED ||
                sdlEvent.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
            {
                App->renderer->WindowResized(sdlEvent.window.data1, sdlEvent.window.data2);
                App->renderer->UpdateBuffers(sdlEvent.window.data1, sdlEvent.window.data2);
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

    return status;
}

bool ModuleInput::CleanUp()
{
    ENGINE_LOG("Quitting SDL input event subsystem");

	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	
    return true;
}
