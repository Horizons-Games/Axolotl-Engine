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
	bool CopyFromOutside(const char* sourceFilePath, const char* destinationFilePath);
	bool Delete(const char* filePath);
	bool Exists(const char* filePath) const;
	bool IsDirectory(const char* directoryPath) const;
	bool CreateDirectory(const char* directoryPath);
	std::vector<std::string> ListFiles(const char* directoryPath);
	long long GetModificationDate(const char* filePath) const;
	const std::string GetPathWithoutFile(const std::string& pathWithFile);
	const std::string GetPathWithoutExtension(const std::string& pathWithExtension);
	const std::string GetFileName(const std::string& path);
	const std::string GetFileExtension(const std::string& path);
	const std::string GetPathWithExtension(const std::string& pathWithoutExtension);
};

