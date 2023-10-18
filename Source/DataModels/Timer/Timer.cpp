#include "StdAfx.h"

#include "Timer.h"

Timer::Timer() : startTick(0), pauseTick(0), stopTick(0), stopped(false)
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

	return static_cast<float>(SDL_GetTicks() - startTick + pauseTick);
}

void Timer::Pause()
{
	pauseTick = static_cast<Uint32>(Read());
}

void Timer::Play()
{
	startTick = SDL_GetTicks();
}

float Timer::Stop()
{
	stopped = true;
	pauseTick = 0;
	stopTick = SDL_GetTicks();

	return static_cast<float>(SDL_GetTicks() - startTick);
}
