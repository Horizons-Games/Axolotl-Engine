#pragma once

#include "Module.h"
#include "SDL/include/SDL.h"

#define TOP_WINDOWED_PADDING 60

class Application;

class ModuleWindow : public Module
{
public:
	ModuleWindow();
	~ModuleWindow() override;

	bool Init() override;
	bool CleanUp() override;

	std::pair<int, int> GetWindowSize() const;
	bool IsWindowFullscreen() const;
	bool IsWindowResizable() const;
	bool IsWindowBorderless() const;
	bool IsWindowDesktopFullscreen() const;
	float GetBrightness() const;
	bool GetVsync();

	void SetWindowSize(int width, int height);
	void SetWindowToDefault();
	void SetFullscreen(bool fullscreen);
	void SetResizable(bool resizable);
	void SetBorderless(bool borderless);
	void SetDesktopFullscreen(bool fullDesktop);
	void SetBrightness(float brightness);
	void SetVsync(bool vsyncactive);

	SDL_Window* GetWindow() const;

private:
	SDL_bool BoolToSDL_Bool(bool boolVal);
	bool IsFlagSet(SDL_WindowFlags flag) const;

	//SDL_Window is incomplete, so we must provide a destructor to the compiler
	struct SDLWindowDestroyer
	{
		void operator()(SDL_Window* window) const
		{
			SDL_DestroyWindow(window);
		}
	};

	//The window we'll be rendering to
	std::unique_ptr<SDL_Window, SDLWindowDestroyer> window;

	//The surface contained by the window
	SDL_Surface* screenSurface;

	//whether the window was set to regular fullscreen (true) or desktop fullscreen (false)
	//this is because both flags are set at the same time, so it's impossible to differentiate otherwise
	bool fullscreen;

	float brightness;

	bool vsync;
};

inline bool ModuleWindow::IsWindowFullscreen() const
{
	return IsFlagSet(SDL_WINDOW_FULLSCREEN) && fullscreen;
}

inline bool ModuleWindow::IsWindowResizable() const
{
	return IsFlagSet(SDL_WINDOW_RESIZABLE);
}

inline bool ModuleWindow::IsWindowBorderless() const
{
	return IsFlagSet(SDL_WINDOW_BORDERLESS);
}

inline bool ModuleWindow::IsWindowDesktopFullscreen() const
{
	return IsFlagSet(SDL_WINDOW_FULLSCREEN_DESKTOP) && !fullscreen;
}

inline float ModuleWindow::GetBrightness() const
{
	return brightness;
}

inline bool ModuleWindow::GetVsync() 
{
	return vsync;
}

inline SDL_Window* ModuleWindow::GetWindow() const
{
	return window.get();
}

inline SDL_bool ModuleWindow::BoolToSDL_Bool(bool boolVal)
{
	return boolVal ? SDL_TRUE : SDL_FALSE;
}

inline bool ModuleWindow::IsFlagSet(SDL_WindowFlags flag) const
{
	Uint32 windowFlags = SDL_GetWindowFlags(GetWindow());
	return windowFlags & flag;
}
