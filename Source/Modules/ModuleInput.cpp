#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleProgram.h"
#include "ModuleEngineCamera.h"
#include "ModuleWindow.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "FileSystem/ModuleResources.h"

#include "imgui_impl_sdl.h"

#include <algorithm>
#include <string>

#include "GL/glew.h"

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

    this->mouseMotion.first = 0.f;
    this->mouseMotion.second = 0.f;
    this->mouseWheelScrolled = false;
    
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        if (this->keysState[i] == KeyState::DOWN)
            this->keysState[i] = KeyState::REPEAT;

        if (this->keysState[i] == KeyState::UP)
            this->keysState[i] = KeyState::IDLE;
    }

    for (int i = 0; i < NUM_MOUSEBUTTONS; ++i) {
        if (this->mouseButtonState[i] == KeyState::DOWN)
            this->mouseButtonState[i] = KeyState::REPEAT;

        if (this->mouseButtonState[i] == KeyState::UP)
            this->mouseButtonState[i] = KeyState::IDLE;
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
            this->keysState[sdlEvent.key.keysym.scancode] = KeyState::DOWN;
            break;
        case SDL_KEYUP:
            this->keysState[sdlEvent.key.keysym.scancode] = KeyState::UP;
            break;

        case SDL_MOUSEBUTTONDOWN:
            this->mouseButtonState[sdlEvent.button.button] = KeyState::DOWN;
            break;
        case SDL_MOUSEBUTTONUP:
            this->mouseButtonState[sdlEvent.button.button] = KeyState::UP;
            break;

        case SDL_MOUSEMOTION:
            this->mouseMotion = std::make_pair(sdlEvent.motion.xrel, sdlEvent.motion.yrel);
            break;

        case SDL_MOUSEWHEEL:
            this->mouseWheel.first = sdlEvent.wheel.x;
            this->mouseWheel.second = sdlEvent.wheel.y;
            this->mouseWheelScrolled = true;
            break;

        case SDL_DROPFILE:
            char* droppedFilePath = sdlEvent.drop.file;

            /*if (App->renderer->IsSupportedPath(droppedFilePath))
                App->renderer->LoadModel(droppedFilePath);*/
            std::string dropFilePath(droppedFilePath);
            std::replace(dropFilePath.begin(), dropFilePath.end(), '\\', '/'); 
            App->scene->GetLoadedScene()->ConvertModelIntoGameObject(droppedFilePath);
            //UID modelUID = App->resources->ImportThread(droppedFilePath);
            // App->renderer->LoadModel(droppedFilePath);
            // UID modelUID = App->resources->ImportThread(droppedFilePath);
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
