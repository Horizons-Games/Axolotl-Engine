#pragma once
#include "Module.h"
#include "physfs.h"

struct zip_t;

class ModuleFileSystem : public Module
{
public:
	ModuleFileSystem();
	~ModuleFileSystem() override;

	bool Init() override;
	bool CleanUp() override;

	void CopyFileInAssets(const std::string& originalPath, const std::string& assetsPath);
	unsigned int Load(const std::string& filePath, char*& buffer) const;
	unsigned int Save(const std::string& filePath, const void* buffer, size_t size, bool append = false) const;
	bool Copy(const std::string& sourceFilePath, const std::string& destinationFilePath) const;
	bool CopyFromOutside(const std::string& sourceFilePath, const std::string& destinationFilePath) const;
	bool Delete(const char* filePath) const;
	bool Exists(const char* filePath) const;
	bool IsDirectory(const char* directoryPath) const;
	bool CreateDirectory(const char* directoryPath) const;
	std::vector<std::string> ListFiles(const char* directoryPath) const;
	std::vector<std::string> ListFilesWithPath(const char* directoryPath);
	long long GetModificationDate(const char* filePath) const;
	const std::string GetPathWithoutFile(const std::string& pathWithFile);
	const std::string GetPathWithoutExtension(const std::string& pathWithExtension);
	const std::string GetFileName(const std::string& path);
	const std::string GetFileExtension(const std::string& path);
	const std::string GetPathWithExtension(const std::string& pathWithoutExtension);

	void SaveInfoMaterial(const std::vector<std::string>& pathTextures, char*& fileBuffer, unsigned int& size);
	void ZipFolder(zip_t* zip, const char* path) const;
	void ZipLibFolder() const;

	void AppendToZipFolder(const std::string& zipPath,
						   const std::string& newFileName,
						   const void* buffer,
						   size_t size,
						   bool overwriteIfExists) const;
	void AppendToZipFolder(const std::string& zipPath, const std::string& existingFilePath) const;

private:
	void DeleteFileInZip(const std::string& zipPath, const std::string& fileName) const;
};

inline bool ModuleFileSystem::Exists(const char* filePath) const
{
	return PHYSFS_exists(filePath);
}

inline bool ModuleFileSystem::IsDirectory(const char* directoryPath) const
{
	return PHYSFS_isDirectory(directoryPath);
}
