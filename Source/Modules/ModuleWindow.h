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
	// Destructor
	virtual ~ModuleWindow();

	// Called before quitting
	bool Init() override;
	// Called before quitting
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

public:
	//The window we'll be rendering to
	SDL_Window* window = nullptr;

	//The surface contained by the window
	SDL_Surface* screenSurface = nullptr;

private:
	bool fullscreen;
	bool borderless;
	bool resizable;
	bool fullscreenDesktop;

	float brightness;
};

#endif // __ModuleWindow_H__