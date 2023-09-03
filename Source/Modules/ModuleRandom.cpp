#include "StdAfx.h"

#include "Modules/ModuleRandom.h"

#include "Application.h"

ModuleRandom::ModuleRandom(unsigned int seed) : mersenneTwisterEngine(seed)
{
	LOG_INFO("ModuleRandom is using seed: {}", seed);
}

ModuleRandom::ModuleRandom() : ModuleRandom(std::random_device()())
{
}

ModuleRandom::~ModuleRandom()
{
}

bool ModuleRandom::RandomChance(float chance)
{
	std::bernoulli_distribution dist(chance);
	return dist(mersenneTwisterEngine);
}

bool ModuleRandom::RandomChanceNormalized(float chance)
{
	float currentFrameRate = 1.f / App->GetDeltaTime();
	float normalizedChance = chance * (currentFrameRate / App->GetMaxFrameRate());
	return RandomChance(normalizedChance);
}

int ModuleRandom::RandomNumberInRange(int maxValue)
{
	return RandomNumberInRange(0, maxValue);
}

int ModuleRandom::RandomNumberInRange(int minValue, int maxValue)
{
	std::uniform_int_distribution<int> dist(minValue, maxValue);
	return dist(mersenneTwisterEngine);
}

float ModuleRandom::RandomNumberInRange(float maxValue)
{
	return RandomNumberInRange(0.f, maxValue);
}

float ModuleRandom::RandomNumberInRange(float minValue, float maxValue)
{
	std::uniform_real_distribution<float> dist(minValue, maxValue);
	return dist(mersenneTwisterEngine);
}
