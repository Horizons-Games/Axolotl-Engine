#include "ModuleFileSystem.h"
#include "physfs.h"
#include <iostream>
#include <cstdio>


bool ModuleFileSystem::Init()
{
    PHYSFS_init(nullptr);
    PHYSFS_mount(".", nullptr, 0);
    PHYSFS_setWriteDir(".");
    return true;
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

std::vector<std::string> ModuleFileSystem::listFiles(const char* directoryPath)
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
