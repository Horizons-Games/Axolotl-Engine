#include "Timer.h"

Timer::Timer() : startTick(0), stopTick(0), stopped(false)
{
}

Timer::~Timer()
{
}

void Timer::Start()
{
	startTick = SDL_GetTicks();
}

float Timer::Read()
{
	if (stopped)
	{
		return (float)(startTick - stopTick);
	}

	return (float)(SDL_GetTicks() - startTick);
}

float Timer::Stop()
{
	stopped = true;
	stopTick = SDL_GetTicks();

	return (float)(SDL_GetTicks() - startTick);
}


