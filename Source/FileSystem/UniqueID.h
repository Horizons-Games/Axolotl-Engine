#pragma once

#include <string>
#include <random>

using UID = unsigned long long;

static std::mt19937_64 CreateMTE64() //Random number engine https://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine
{
    std::random_device randNum;
    constexpr std::size_t n = std::mt19937_64::state_size * sizeof(std::mt19937_64::result_type);
    std::random_device::result_type randomData[(n - 1) / sizeof(randNum()) + 1];
    std::generate(std::begin(randomData), std::end(randomData), std::ref(randNum));
    std::seed_seq seed(std::begin(randomData), std::end(randomData));
    return std::mt19937_64(seed);
}

static std::mt19937_64 creator = CreateMTE64();
static std::uniform_int_distribution<UID> uidNew;
static std::uniform_int_distribution<> uuidNew(0, 15);

class UniqueID 
{
public:
	~UniqueID();

	static UID GenerateUID();
	static std::string GenerateUUID();
};
