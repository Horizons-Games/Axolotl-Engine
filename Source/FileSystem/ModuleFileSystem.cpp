#include "ModuleFileSystem.h"
#include "physfs.h"
#include <iostream>
#include <cstdio>
#include "zip.h"


bool ModuleFileSystem::Init()
{
    PHYSFS_init(nullptr);
    PHYSFS_mount(".", nullptr, 0);
    PHYSFS_setWriteDir(".");
    return true;
}

void ModuleFileSystem::CopyFileInAssets(const std::string& originalPath, const std::string& assetsPath)
{
    //for more protection
    bool exists = Exists(assetsPath.c_str());
    if (!exists)
    {
        CopyFromOutside(originalPath.c_str(), assetsPath.c_str());
    }
}

bool ModuleFileSystem::CopyFromOutside(const char* sourceFilePath, const char* destinationFilePath)
{
    FILE* src, * dst;
    char buffer[4096];
    size_t n;

    src = fopen(sourceFilePath, "rb");
    dst = fopen(destinationFilePath, "wb");

    while ((n = fread(buffer, 1, sizeof buffer, src)) > 0)
    {
        fwrite(buffer, 1, n, dst);
    }
    fclose(src);
    fclose(dst);
    return true;
}

bool ModuleFileSystem::Copy(const char* sourceFilePath, const char* destinationFilePath)
{
    char* buffer = nullptr;
    unsigned int size = Load(sourceFilePath, buffer);
    Save(destinationFilePath, buffer, size, false);
    return true;
}

bool  ModuleFileSystem::Delete(const char* filePath)
{
    if (!PHYSFS_delete(filePath))
    {
        ENGINE_LOG("Physfs fails with error: %s", PHYSFS_getLastError());
        return false;
    }
    return true;
   
}

unsigned int ModuleFileSystem::Load(const char* filePath, char*& buffer) const
{
    PHYSFS_File * file = PHYSFS_openRead(filePath);
    if (file == NULL)
    {
        ENGINE_LOG("Physfs fails with error: %s", PHYSFS_getLastError());
        PHYSFS_close(file);
        return -1;
    }
    PHYSFS_sint64 size = PHYSFS_fileLength(file);
    buffer = new char[size + 1]{};
    if (PHYSFS_readBytes(file, buffer, size) < size)
    {
        ENGINE_LOG("Physfs fails with error: %s", PHYSFS_getLastError());
        PHYSFS_close(file);
        return -1;
    }
    PHYSFS_close(file);
    return size;
}

unsigned int ModuleFileSystem::Save(const char* filePath, const void* buffer, unsigned int size, bool append) const
{
    PHYSFS_File* file = append ? PHYSFS_openAppend(filePath) : PHYSFS_openWrite(filePath);
    if (file == NULL)
    {
        ENGINE_LOG("Physfs fails with error: %s", PHYSFS_getLastError());
        PHYSFS_close(file);
        return 1;
    }
    if (PHYSFS_writeBytes(file, buffer, size) < size)
    {
        ENGINE_LOG("Physfs fails with error: %s", PHYSFS_getLastError());
        PHYSFS_close(file);
        return 1;
    }
    PHYSFS_close(file);
    return 0;
}

bool ModuleFileSystem::Exists(const char* filePath) const
{
    return PHYSFS_exists(filePath);
}

bool ModuleFileSystem::IsDirectory(const char* directoryPath) const
{
    return PHYSFS_isDirectory(directoryPath);
}

bool  ModuleFileSystem::CreateDirectory(const char* directoryPath)
{
    if(!PHYSFS_mkdir(directoryPath)) 
    {
        ENGINE_LOG("Physfs fails with error: %s", PHYSFS_getLastError());
        return false;
    }
    return true;
}

bool ModuleFileSystem::CleanUp() {
    PHYSFS_deinit();
    return true;
}

std::vector<std::string> ModuleFileSystem::ListFiles(const char* directoryPath)
{
    std::vector< std::string> files;
    char **rc = PHYSFS_enumerateFiles(directoryPath);
    char **i;
    for (i = rc; *i != NULL; i++)
    {
        files.push_back(*i);
    }
    PHYSFS_freeList(rc);
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
    for (int i = pathWithFile.size() - 1; 0 <= i && !separatorFound; --i)
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
    int posOfExtensionInPath = pathWithExtension.find(fileExtension);
    if (0 < posOfExtensionInPath) //has file extension
    {
        pathWithoutExtension.erase(posOfExtensionInPath, fileExtension.size());
    }
    return pathWithoutExtension;
}

const std::string ModuleFileSystem::GetFileName(const std::string& path)
{
    std::string fileName = "";
    bool separatorNotFound = true;
    for (int i = path.size() - 1; 0 <= i && separatorNotFound; --i)
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
    for (int i = path.size() - 1; dotNotFound && 0 <= i; --i)
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
    return "";
}

std::vector<std::string> ModuleFileSystem::listFiles(const char* directoryPath)
{
    std::vector< std::string> files;
    char** rc = PHYSFS_enumerateFiles(directoryPath);
    char** i;
    for (i = rc; *i != NULL; i++)
    {
        files.push_back(*i);
    }
    PHYSFS_freeList(rc);
    return files;
}

void ModuleFileSystem::ZipFolder(struct zip_t* zip, const char* path)
{
    std::vector<std::string> files = listFiles(path);
    for (int i = 0; i < files.size(); ++i)
    {
        std::string newPath(path);
        newPath += '/' + files[i];
        if (IsDirectory(newPath.c_str())) ZipFolder(zip, newPath.c_str());
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