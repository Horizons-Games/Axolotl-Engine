#pragma once

#include "SDL/include/SDL.h"

class Timer
{
public:
	Timer();
	~Timer();

	void Start();
	float Read();
	float Stop();
	void Pause();
	void Play();

private:
	Uint32 startTick;
	Uint32 stopTick;
	Uint32 pauseTick;

	bool stopped;
};

inline void Timer::Start()
{
	startTick = SDL_GetTicks();
	stopped = false;
}
