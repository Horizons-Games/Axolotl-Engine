#include "FileSystem/UIDGenerator.h"

#include <random>
#include <sstream>

namespace UniqueID
{
namespace
{
// Random number engine https://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine
std::mt19937_64 CreateMTE64()
{
	std::random_device randNum;
	constexpr std::size_t n = std::mt19937_64::state_size * sizeof(std::mt19937_64::result_type);
	std::random_device::result_type randomData[(n - 1) / sizeof(randNum()) + 1];
	std::generate(std::begin(randomData), std::end(randomData), std::ref(randNum));
	std::seed_seq seed(std::begin(randomData), std::end(randomData));
	return std::mt19937_64(seed);
}

std::mt19937_64 creator = CreateMTE64();
std::uniform_int_distribution<UID> uidNew;

} // namespace

UID GenerateUID()
{
	return uidNew(creator);
}

} // namespace UniqueID
