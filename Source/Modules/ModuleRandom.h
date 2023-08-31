#pragma once

#include "Modules/Module.h"

class ModuleRandom : public Module
{
public:
	ModuleRandom(unsigned int seed);
	ModuleRandom();
	~ModuleRandom() override;

	// Returns true with a probability of 1 in 1 / chance
	// For example, if chance = 0.25f, the probability is 1 in 4
	bool RandomChance(float chance);
	// Returns true with a probability of 1 in 1 / (chance * (currentFrameRate / maxFramerate)), and returns true if
	// the number is 0 In other words, the chance is normalized so it is 1 in "chance" to happen when running at max
	// frames per second
	bool RandomChanceNormalized(float chance);

	// Generates a random number in the interval [0, maxValue]
	int RandomNumberInRange(int maxValue);
	// Generates a random number in the interval [minValue, maxValue]
	int RandomNumberInRange(int minValue, int maxValue);

	// Generates a random number in the interval [0.f, maxValue)
	float RandomNumberInRange(float maxValue);
	// Generates a random number in the interval [minValue, maxValue)
	float RandomNumberInRange(float minValue, float maxValue);

private:
	std::mt19937 mersenneTwisterEngine;
};
