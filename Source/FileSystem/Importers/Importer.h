#pragma once

#include "SDL/include/SDL.h"
#include <memory>

template <typename U>

class Importer
{
public:

	Importer()
	{
	}
	// Reads a path that indicates a file to import to engine
	virtual void Import(const char* filePath, std::shared_ptr<U> resource) = 0;
	// Reads your own data and generates a file buffer
	virtual uint64_t Save(const std::shared_ptr<U> resource, char*& fileBuffer) = 0;
	// Reads assimp data and generates your own data
	virtual void Load(const char* fileBuffer, std::shared_ptr<U> resource) = 0;

};