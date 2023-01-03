#include "UID.h"

#include <random>

static std::random_device random;
static std::mt19937_64 mersenne_twister(random());
static std::uniform_int_distribution<UID> distribution;

UID GenerateUID()
{
	return distribution(mersenne_twister);
}
