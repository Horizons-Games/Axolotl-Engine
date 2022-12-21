#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

ModuleWindow::ModuleWindow()
{
}

// Destructor
ModuleWindow::~ModuleWindow()
{
}

// Called before render is available
bool ModuleWindow::Init()
{
	ENGINE_LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		ENGINE_LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		SDL_DisplayMode DM;
		SDL_GetCurrentDisplayMode(0, &DM);
		int width = DM.w;
		int height = DM.h - TOP_WINDOWED_PADDING;

		this->fullscreen = FULLSCREEN;
		this->borderless = BORDERLESS;
		this->resizable = RESIZABLE;
		this->fullscreenDesktop = FULLSCREEN_DESKTOP;
		this->brightness = BRIGHTNESS;

		Uint32 flags = SDL_WINDOW_SHOWN |  SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED;

		if (this->fullscreen)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}
		if (this->borderless)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}
		if (this->resizable)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}
		if (this->fullscreenDesktop)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			width, height, flags);

		if(window == NULL)
		{
			ENGINE_LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

			//Get window surface
			screenSurface = SDL_GetWindowSurface(window);
		}
	}

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	ENGINE_LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

std::pair<int, int> ModuleWindow::GetWindowSize() const
{
	int width, height;

	SDL_GetWindowSize(this->window, &width, &height);

	return std::make_pair(width, height);
}

bool ModuleWindow::GetFullscreen() const
{
	return this->fullscreen;
}

bool ModuleWindow::GetBorderless() const
{
	return this->borderless;
}

bool ModuleWindow::GetResizable() const
{
	return this->resizable;
}

bool ModuleWindow::GetFulscreenDesktop() const
{
	return this->fullscreenDesktop;
}

float ModuleWindow::GetBrightness() const
{
	return this->brightness;
}

void ModuleWindow::SetWindowSize(int width, int height)
{
	SDL_SetWindowSize(this->window, width, height);
}

void ModuleWindow::SetWindowType(bool fullscreen, bool borderless,
	bool resizable, bool fullscreenDesktop)
{
	ENGINE_LOG("---- Changing window mode ----")

	this->fullscreen = fullscreen;
	this->borderless = borderless;
	this->resizable = resizable;
	this->fullscreenDesktop = fullscreenDesktop;

	if (this->fullscreen)
		SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN);
	else if (this->fullscreenDesktop)
		SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(this->window, 0);

	if (this->borderless)
		SDL_SetWindowBordered(this->window, SDL_FALSE);
	else
		SDL_SetWindowBordered(this->window, SDL_TRUE);

	if (this->resizable)
		SDL_SetWindowResizable(this->window, SDL_TRUE);
	else
		SDL_SetWindowResizable(this->window, SDL_FALSE);
}

void ModuleWindow::SetBrightness(float brightness)
{
	this->brightness = brightness;

	if (SDL_SetWindowBrightness(this->window, brightness))
	{
		ENGINE_LOG("Error setting window brightness: %s", &SDL_GetError()[0]);
	}
}



