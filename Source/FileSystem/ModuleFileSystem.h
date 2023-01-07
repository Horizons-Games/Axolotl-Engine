#pragma once
#include "Module.h"

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem() = default;
	~ModuleFileSystem() = default;
	
	unsigned int Load(const char* filePath, char*& buffer) const;
	unsigned int Save(const char* filePath, const void* buffer, unsigned int size, bool append = false) const;
	bool Exists(const char* filePath) const;
	bool IsDirectory(const char* directoryPath) const;
	bool CreateDirectory(const char* directoryPath);
};

