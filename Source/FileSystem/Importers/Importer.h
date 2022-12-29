#pragma once

#include "SDL/include/SDL.h"

template <typename T, typename U>

class Importer
{
public:

	Importer()
	{
	}
	// Reads assimp data and generates your own data
	virtual void Import(const T* material, U* ourMaterial) = 0;
	// Reads your own data and generates a file buffer
	virtual uint64_t Save(const U* ourMaterial, char** fileBuffer) = 0;
	// Reads a file buffer and generates your own data
	virtual void Load(const char* fileBuffer, U* ourMaterial) = 0;
};