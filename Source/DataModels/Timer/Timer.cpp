#include "StdAfx.h"

#include "Timer.h"

Timer::Timer() : startTick(0), stopTick(0), stopped(false)
{
}

Timer::~Timer()
{
}

float Timer::Read()
{
	if (stopped)
	{
		return static_cast<float>(startTick - stopTick);
	}

	return static_cast<float>(SDL_GetTicks() - startTick);
}

float Timer::Stop()
{
	stopped = true;
	stopTick = SDL_GetTicks();

	return static_cast<float>(SDL_GetTicks() - startTick);
}
