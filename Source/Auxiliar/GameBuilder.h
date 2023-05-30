#pragma once

namespace builder
{
enum class BuildType
{
	DEBUG_GAME,
	RELEASE_GAME
};
void BuildGame(BuildType buildType, bool generateZip = true);

void Terminate();
bool Compiling();
bool Zipping();
} // namespace builder