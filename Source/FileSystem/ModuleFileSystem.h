#pragma once
#include "Module.h"

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem() = default;
	~ModuleFileSystem() = default;
	
	unsigned int Load(const char* file_path, char** buffer) const;
	unsigned int Save(const char* file_path, const void* buffer, unsigned int size, bool append = false) const;
	bool Exists(const char* file_path) const;
};

