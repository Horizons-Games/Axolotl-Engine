#include "StdAfx.h"

#include "ModuleFileSystem.h"
#include "physfs.h"
#include "zip.h"

#include "FileSystem/FileZippedData.h"
#include "FileSystem/UIDGenerator.h"

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

bool ModuleFileSystem::CleanUp()
{
#ifdef ENGINE
	logContext->StopWritingToFile();
#endif // ENGINE
	// returns non-zero on success, zero on failure
	int deinitResult = PHYSFS_deinit();
	return deinitResult != 0;
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

bool ModuleFileSystem::Exists(const char* filePath) const
{
	return PHYSFS_exists(filePath);
}

bool ModuleFileSystem::IsDirectory(const char* directoryPath) const
{
	return PHYSFS_isDirectory(directoryPath);
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

unsigned int
	ModuleFileSystem::Save(const std::string& filePath, const void* buffer, size_t size, bool append /*= false*/) const
{
	PHYSFS_File* file = append ? PHYSFS_openAppend(filePath.c_str()) : PHYSFS_openWrite(filePath.c_str());
	if (file == nullptr)
	{
		LOG_ERROR("Physfs has error {{}} when try to save {}", PHYSFS_getLastError(), filePath);
		PHYSFS_close(file);
		return 1;
	}
	if (PHYSFS_writeBytes(file, buffer, size) < static_cast<PHYSFS_sint64>(size))
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

std::size_t ModuleFileSystem::CountTotalFiles(const std::string& path) const
{
	std::vector<std::string> allItemsInFolder = ListFiles(path.c_str());
	auto folders = allItemsInFolder | std::views::filter(
										  [&](const std::string& itemPath)
										  {
											  return IsDirectory((path + '/' + itemPath).c_str());
										  });
	std::size_t totalItems = 0U;
	std::size_t numberOfFolders = 0U;
	for (const std::string& folderPath : folders)
	{
		totalItems += CountTotalFiles(path + '/' + folderPath);
		++numberOfFolders;
	}
	// Add the number of items that were not folders
	totalItems += allItemsInFolder.size() - numberOfFolders;

	return totalItems;
}

void ModuleFileSystem::ZipFolder(zip_t* zip, const char* path) const
{
	std::size_t initialFile = 0U;
	ZipFolderRecursive(zip, path, path, CountTotalFiles(path), initialFile);
}

void ModuleFileSystem::ZipLibFolder() const
{
	zip_t* zip = zip_open("Assets.zip", ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');
	ZipFolder(zip, "Lib");
	ZipFolder(zip, "WwiseProject");
	zip_close(zip);
}

void ModuleFileSystem::AppendToZipFolder(const std::string& zipPath,
										 const std::string& newFileName,
										 const void* buffer,
										 size_t size,
										 bool overwriteIfExists) const
{
	if (overwriteIfExists)
	{
		DeleteFileInZip(zipPath, newFileName);
	}
	zip_t* zip = zip_open(zipPath.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'a');
	zip_entry_open(zip, newFileName.c_str());
	zip_entry_write(zip, buffer, size);
	zip_entry_close(zip);
	zip_close(zip);
}

void ModuleFileSystem::AppendToZipFolder(const std::string& zipPath, const std::string& existingFilePath) const
{
	char* buffer = nullptr;
	unsigned int size = Load(existingFilePath, buffer);
	AppendToZipFolder(zipPath, existingFilePath, buffer, size, true);
	delete buffer;
}

ConnectedCallback ModuleFileSystem::RegisterFileZippedCallback(FileZippedCallback&& callback)
{
	std::scoped_lock(callbacksMutex);
	UID callbackUID = UniqueID::GenerateUID();
	callbacks[callbackUID] = std::move(callback);
	return ConnectedCallback(std::bind(&ModuleFileSystem::DeregisterFileZippedCallback, this, callbackUID));
}

void ModuleFileSystem::DeregisterFileZippedCallback(UID callbackUID)
{
	std::scoped_lock(callbacksMutex);
	auto callbackToDelete = callbacks.find(callbackUID);
	if (callbackToDelete != std::end(callbacks))
	{
		callbacks.erase(callbackToDelete);
	}
}

void ModuleFileSystem::ZipFolderRecursive(
	zip_t* zip, const char* path, const std::string& rootPath, std::size_t totalItems, std::size_t& currentItem) const
{
	std::vector<std::string> files = ListFiles(path);
	for (std::size_t i = 0; i < files.size(); ++i)
	{
		std::string itemPath(path);
		itemPath += '/' + files[i];
		if (IsDirectory(itemPath.c_str()))
		{
			ZipFolderRecursive(zip, itemPath.c_str(), rootPath, totalItems, currentItem);
		}
		else
		{
			std::chrono::time_point zipFileStart = std::chrono::steady_clock::now();
			zip_entry_open(zip, itemPath.c_str());
			{
				char* buf = nullptr;
				unsigned int size = Load(itemPath.c_str(), buf);
				zip_entry_write(zip, buf, size);
				delete buf;
			}
			zip_entry_close(zip);
			std::chrono::duration<float> zipFileDuration = std::chrono::steady_clock::now() - zipFileStart;

			FileZippedData fileZippedData{ itemPath, currentItem, std::move(zipFileDuration), rootPath, totalItems };
			std::scoped_lock(callbacksMutex);
			for (const auto& [_, callback] : callbacks)
			{
				callback(fileZippedData);
			}

			++currentItem;
		}
	}
}

void ModuleFileSystem::DeleteFileInZip(const std::string& zipPath, const std::string& fileName) const
{
	zip_t* zip = zip_open(zipPath.c_str(), ZIP_DEFAULT_COMPRESSION_LEVEL, 'a');

	// create a char* const* with only the fileName inside
	// because it'd be boring if it was simple

	char* charPtr = new char[fileName.length() + 1];
	std::strcpy(charPtr, fileName.c_str());

	char* const charArray[] = { charPtr };

	zip_entries_delete(zip, charArray, 1);

	delete[] charPtr;

	zip_close(zip);
}
