#pragma once

#include "SDL/include/SDL.h"

class Importer
{
public:

	Importer()
	{
	}
	// Reads assimp data and generates your own data
	virtual void Import(...) = 0;
	// Reads your own data and generates a file buffer
	virtual uint64_t Save(...) = 0;
	// Reads a file buffer and generates your own data
	virtual void Load(...) = 0;
};