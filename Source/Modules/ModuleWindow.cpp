#include "ModuleWindow.h"

ModuleWindow::ModuleWindow() : fullscreen (false), brightness (0.0f)
{
}

ModuleWindow::~ModuleWindow()
{
}

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

		brightness = BRIGHTNESS;

		Uint32 flags = SDL_WINDOW_SHOWN |  SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED;

		if (FULLSCREEN)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		SDL_Window* windowRawPointer = SDL_CreateWindow(TITLE, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			width, height, flags);
		window = std::unique_ptr<SDL_Window, SDLWindowDestroyer>(windowRawPointer);

		if(window == NULL)
		{
			ENGINE_LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			SDL_EventState(SDL_DROPFILE, SDL_ENABLE);

			//Get window surface
			SDL_Surface* sufaceRawPointer = SDL_GetWindowSurface(window.get());
			screenSurface = std::unique_ptr<SDL_Surface, SDLSurfaceDestroyer>(sufaceRawPointer);
		}
	}

	return ret;
}

bool ModuleWindow::CleanUp()
{
	ENGINE_LOG("Destroying SDL window and quitting all SDL systems");

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

std::pair<int, int> ModuleWindow::GetWindowSize() const
{
	int width, height;

	SDL_GetWindowSize(GetWindow(), &width, &height);

	return std::make_pair(width, height);
}

void ModuleWindow::SetWindowSize(int width, int height)
{
	SDL_SetWindowSize(GetWindow(), width, height);
}

void ModuleWindow::SetWindowToDefault()
{
	ENGINE_LOG("---- Changing window mode ----");

	SDL_SetWindowFullscreen(GetWindow(), 0);
	SDL_SetWindowResizable(GetWindow(), SDL_FALSE);
	SDL_SetWindowBordered(GetWindow(), SDL_TRUE);
}

void ModuleWindow::SetFullscreen(bool fullscreen)
{
	SetWindowToDefault();
	if (fullscreen) {
		SDL_SetWindowFullscreen(GetWindow(), SDL_WINDOW_FULLSCREEN);
		this->fullscreen = true;
	}
}

void ModuleWindow::SetResizable(bool resizable)
{
	SetWindowToDefault();
	SDL_SetWindowResizable(GetWindow(), BoolToSDL_Bool(resizable));
}

void ModuleWindow::SetBorderless(bool borderless)
{
	SetWindowToDefault();
	//this call sets borders, so it's the opposite of what we want
	//thus the negation
	SDL_SetWindowBordered(GetWindow(), BoolToSDL_Bool(!borderless));
}

void ModuleWindow::SetDesktopFullscreen(bool fullDesktop)
{
	SetWindowToDefault();
	if (fullDesktop) {
		SDL_SetWindowFullscreen(GetWindow(), SDL_WINDOW_FULLSCREEN_DESKTOP);
		fullscreen = false;
	}
}

void ModuleWindow::SetBrightness(float brightness)
{
	this->brightness = brightness;

	if (SDL_SetWindowBrightness(GetWindow(), brightness))
	{
		ENGINE_LOG("Error setting window brightness: %s", &SDL_GetError()[0]);
	}
}

SDL_bool ModuleWindow::BoolToSDL_Bool(bool boolVal)
{
	if (boolVal)
	{
		return SDL_TRUE;
	}
	return SDL_FALSE;
}



