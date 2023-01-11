#pragma once
#include "Module.h"
#include <vector>

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem() = default;
	~ModuleFileSystem() = default;
	bool Init() override;
	bool CleanUp() override;
	unsigned int Load(const char* filePath, char*& buffer) const;
	unsigned int Save(const char* filePath, const void* buffer, unsigned int size, bool append = false) const;
	bool Copy(const char* sourceFilePath, const char* destinationFilePath);
	bool Delete(const char* filePath);
	bool Exists(const char* filePath) const;
	bool IsDirectory(const char* directoryPath) const;
	bool CreateDirectory(const char* directoryPath);
	std::vector<std::string> listFiles(const char* directoryPath);
};

