#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

#define TOP_WINDOWED_PADDING 60

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow();
	virtual ~ModuleWindow();

	bool Init() override;
	bool CleanUp() override;

	std::pair<int, int> GetWindowSize() const;
	bool GetFullscreen() const;
	bool GetBorderless() const;
	bool GetResizable() const;
	bool GetFulscreenDesktop() const;
	float GetBrightness() const;

	void SetWindowSize(int width, int height);
	void SetWindowType(bool fullscreen, bool borderless, bool resizable, bool fullscreenDesktop);
	void SetBrightness(float brightness);

	inline SDL_Window* GetWindow() const
	{
		return window.get();
	}

private:
	//SDL_Window is incomplete, so we must provide a destructor to the compiler
	struct SDLWindowDestroyer
	{
		void operator()(SDL_Window* window) const
		{
			SDL_DestroyWindow(window);
		}
	};
	//SDL_Surface is also incomplete
	struct SDLSurfaceDestroyer
	{
		void operator()(SDL_Surface* surface) const
		{
			SDL_FreeSurface(surface);
		}
	};

	//The window we'll be rendering to
	std::unique_ptr<SDL_Window, SDLWindowDestroyer> window;

	//The surface contained by the window
	std::unique_ptr<SDL_Surface, SDLSurfaceDestroyer> screenSurface;

	bool fullscreen;
	bool borderless;
	bool resizable;
	bool fullscreenDesktop;

	float brightness;
};

#endif // __ModuleWindow_H__