#include "ModuleFileSystem.h"
#include "physfs.h"
#include "zip.h"
#ifndef ENGINE
	#include <assert.h>
#endif

ModuleFileSystem::ModuleFileSystem()
{
}

ModuleFileSystem::~ModuleFileSystem()
{
}

bool ModuleFileSystem::Init()
{
	PHYSFS_init(nullptr);
	PHYSFS_mount(".", nullptr, 0);
#ifdef ENGINE
	PHYSFS_mount("..", nullptr, 0);
	PHYSFS_setWriteDir(".");
	logContext->StartWritingToFile();
#else  // ENGINE
	if (!Exists("Assets.zip"))
	{
		assert(false && "Binary zip not found!");
		return false;
	}
	PHYSFS_mount("Assets.zip", nullptr, 0);
	PHYSFS_unmount(".");
#endif // GAME
	return true;
}

void ModuleFileSystem::CopyFileInAssets(const std::string& originalPath, const std::string& assetsPath)
{
	// for more protection
	bool exists = Exists(assetsPath.c_str());
	if (!exists)
	{
		if (!Copy(originalPath.c_str(), assetsPath.c_str()))
		{
			CopyFromOutside(originalPath.c_str(), assetsPath.c_str());
		}
	}
}

bool ModuleFileSystem::CopyFromOutside(const std::string& sourceFilePath, const std::string& destinationFilePath) const
{
	FILE *src, *dst;
	char buffer[4096];
	size_t n;

	src = fopen(sourceFilePath.c_str(), "rb");
	dst = fopen(destinationFilePath.c_str(), "wb");

	while ((n = fread(buffer, 1, sizeof buffer, src)) > 0)
	{
		fwrite(buffer, 1, n, dst);
	}
	fclose(src);
	fclose(dst);
	return true;
}

bool ModuleFileSystem::Copy(const std::string& sourceFilePath, const std::string& destinationFilePath) const
{
	char* buffer = nullptr;
	unsigned int size = Load(sourceFilePath, buffer);
	if (size == -1)
	{
		return false;
	}
	Save(destinationFilePath, buffer, size, false);
	delete buffer;
	return true;
}

bool ModuleFileSystem::Delete(const char* filePath) const
{
	if (!PHYSFS_delete(filePath))
	{
		LOG_ERROR("Physfs has error {{}} when try to delete {}", PHYSFS_getLastError(), filePath);
		return false;
	}
	return true;
}

unsigned int ModuleFileSystem::Load(const std::string& filePath, char*& buffer) const
{
	PHYSFS_File* file = PHYSFS_openRead(filePath.c_str());
	if (file == nullptr)
	{
		LOG_ERROR("Physfs has error {{}} when try to open {}", PHYSFS_getLastError(), filePath);
		PHYSFS_close(file);
		return 0;
	}
	PHYSFS_sint64 size = PHYSFS_fileLength(file);
	buffer = new char[size + 1]{};
	if (PHYSFS_readBytes(file, buffer, size) < size)
	{
		LOG_ERROR("Physfs has error {{}} when try to open {}", PHYSFS_getLastError(), filePath);
		PHYSFS_close(file);
		return 0;
	}
	PHYSFS_close(file);
	return (unsigned int) size;
}

unsigned int ModuleFileSystem::Save(const std::string& filePath,
									const void* buffer,
									unsigned int size,
									bool append /*= false*/) const
{
	PHYSFS_File* file = append ? PHYSFS_openAppend(filePath.c_str()) : PHYSFS_openWrite(filePath.c_str());
	if (file == nullptr)
	{
		LOG_ERROR("Physfs has error {{}} when try to save {}", PHYSFS_getLastError(), filePath);
		PHYSFS_close(file);
		return 1;
	}
	if (PHYSFS_writeBytes(file, buffer, size) < size)
	{
		LOG_ERROR("Physfs has error {{}} when try to save {}", PHYSFS_getLastError(), filePath);
		PHYSFS_close(file);
		return 1;
	}
	PHYSFS_close(file);
	return 0;
}

bool ModuleFileSystem::CreateDirectory(const char* directoryPath) const
{
	if (!PHYSFS_mkdir(directoryPath))
	{
		LOG_ERROR("Physfs has error {{}} when try to create {}", PHYSFS_getLastError(), directoryPath);
		return false;
	}
	return true;
}

std::vector<std::string> ModuleFileSystem::ListFiles(const char* directoryPath) const
{
	std::vector<std::string> files;
	char** rc = PHYSFS_enumerateFiles(directoryPath);
	char** i;
	for (i = rc; *i != NULL; i++)
	{
		files.push_back(*i);
	}
	PHYSFS_freeList(rc);
	return files;
}

std::vector<std::string> ModuleFileSystem::ListFilesWithPath(const char* directoryPath)
{
	std::vector<std::string> files = ListFiles(directoryPath);
	for (int i = 0; i < files.size(); i++)
	{
		files[i] = directoryPath + files[i];
	}
	return files;
}

long long ModuleFileSystem::GetModificationDate(const char* filePath) const
{
	PHYSFS_Stat fileStats;
	PHYSFS_stat(filePath, &fileStats);
	return fileStats.modtime;
}

const std::string ModuleFileSystem::GetPathWithoutFile(const std::string& pathWithFile)
{
	std::string fileName = "";
	bool separatorFound = false;
	for (int i = (int) (pathWithFile.size() - 1); 0 <= i && !separatorFound; --i)
	{
		char currentChar = pathWithFile[i];
		separatorFound = currentChar == '\\' || currentChar == '/';
		if (separatorFound)
		{
			fileName = pathWithFile.substr(0, i + 1);
		}
	}
	return fileName;
}

const std::string ModuleFileSystem::GetPathWithoutExtension(const std::string& pathWithExtension)
{
	std::string pathWithoutExtension = std::string(pathWithExtension);
	std::string fileExtension = GetFileExtension(pathWithExtension);
	int posOfExtensionInPath = (int) pathWithExtension.find(fileExtension);
	if (0 < posOfExtensionInPath) // has file extension
	{
		pathWithoutExtension.erase(posOfExtensionInPath, fileExtension.size());
	}
	return pathWithoutExtension;
}

const std::string ModuleFileSystem::GetFileName(const std::string& path)
{
	std::string fileName = "";
	bool separatorNotFound = true;
	for (int i = (int) (path.size() - 1); 0 <= i && separatorNotFound; --i)
	{
		char currentChar = path[i];
		separatorNotFound = currentChar != '\\' && currentChar != '/';
		if (separatorNotFound)
		{
			fileName.insert(0, 1, currentChar);
		}
	}
	return GetPathWithoutExtension(fileName);
}

const std::string ModuleFileSystem::GetFileExtension(const std::string& path)
{
	std::string fileExtension = "";
	bool dotNotFound = true;
	for (int i = (int) (path.size() - 1); dotNotFound && 0 <= i; --i)
	{
		char currentChar = path[i];
		fileExtension.insert(fileExtension.begin(), currentChar);
		dotNotFound = currentChar != '.';
	}
	return fileExtension;
}

const std::string ModuleFileSystem::GetPathWithExtension(const std::string& pathWithoutExtension)
{
	std::string filePath = GetPathWithoutFile(pathWithoutExtension);
	std::string fileName = GetFileName(pathWithoutExtension);
	std::vector<std::string> files = ListFiles(filePath.c_str());
	for (size_t i = 0; i < files.size(); i++)
	{
		std::string currentFile = files[i];
		if (GetPathWithoutExtension(currentFile) == fileName)
		{
			std::string path(filePath);
			path += currentFile;
			if (GetFileExtension(path) != ".meta")
			{
				return path;
			}
		}
	}
	return std::string();
}

void ModuleFileSystem::SaveInfoMaterial(const std::vector<std::string>& pathTextures,
										char*& fileBuffer,
										unsigned int& size)
{
	unsigned int header[4] = { (unsigned int) pathTextures[0].size(),
							   (unsigned int) pathTextures[1].size(),
							   (unsigned int) pathTextures[2].size(),
							   (unsigned int) pathTextures[3].size() };

	size = (unsigned int) (sizeof(header) + pathTextures[0].size() + pathTextures[1].size() + pathTextures[2].size() +
						   pathTextures[3].size());

	char* cursor = new char[size]{};

	fileBuffer = cursor;

	unsigned int bytes = sizeof(header);
	memcpy(cursor, header, bytes);

	cursor += bytes;

	bytes = (unsigned int) pathTextures[0].size();
	memcpy(cursor, pathTextures[0].c_str(), bytes);

	cursor += bytes;

	bytes = (unsigned int) pathTextures[1].size();
	memcpy(cursor, pathTextures[1].c_str(), bytes);

	cursor += bytes;

	bytes = (unsigned int) pathTextures[2].size();
	memcpy(cursor, pathTextures[2].c_str(), bytes);

	cursor += bytes;

	bytes = (unsigned int) pathTextures[3].size();
	memcpy(cursor, pathTextures[3].c_str(), bytes);
}

void ModuleFileSystem::ZipFolder(struct zip_t* zip, const char* path) const
{
	std::vector<std::string> files = ListFiles(path);
	for (int i = 0; i < files.size(); ++i)
	{
		std::string newPath(path);
		newPath += '/' + files[i];
		if (IsDirectory(newPath.c_str()))
		{
			ZipFolder(zip, newPath.c_str());
		}
		else
		{
			zip_entry_open(zip, newPath.c_str());
			{
				char* buf = nullptr;
				PHYSFS_File* file = PHYSFS_openRead(newPath.c_str());
				PHYSFS_sint64 size = PHYSFS_fileLength(file);
				Load(newPath.c_str(), buf);
				zip_entry_write(zip, buf, size);
			}
			zip_entry_close(zip);
		}
	}
}

void ModuleFileSystem::ZipLibFolder() const
{
	struct zip_t* zip = zip_open("Assets.zip", ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
	ZipFolder(zip, "Lib");
    ZipFolder(zip, "WwiseProject");
	zip_close(zip);
}
