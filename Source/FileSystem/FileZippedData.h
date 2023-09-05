#pragma once

#include <cstddef>
#include <chrono>

struct FileZippedData
{
	std::string fileZipped;
	std::size_t fileZippedIndex;
	std::chrono::duration<float> timeTaken;
	std::string folderBeingZipped;
	std::size_t totalFiles;
};
