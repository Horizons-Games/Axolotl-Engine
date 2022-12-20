#pragma once

#include "SDL/include/SDL.h"

#include <vector>

class Timer
{
public:
	Timer();
	~Timer();

	void Start();
	float Read();
	float Stop();

private:
	Uint32 startTick;
	Uint32 stopTick;

	bool stopped;
};

