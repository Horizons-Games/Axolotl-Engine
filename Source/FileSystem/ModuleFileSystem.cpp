#include "ModuleFileSystem.h"
#include "physfs.h"
#include <fstream>
#include <vector>
#include <cstring>
#include <direct.h>
#include <cstdio>


bool ModuleFileSystem::Start() {
    PHYSFS_init(nullptr);
    PHYSFS_mount(".", nullptr, 0);
    PHYSFS_setWriteDir(".");
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
        ENGINE_LOG("Physfs fails with error: %s", PHYSFS_getLastErrorCode());
        return false;
    }
    return true;
   
}

unsigned int ModuleFileSystem::Load(const char* filePath, char*& buffer) const
{
    PHYSFS_File * file = PHYSFS_openRead(filePath);
    if (file == NULL)
    {
        ENGINE_LOG("Physfs fails with error: %s", PHYSFS_getLastErrorCode());
        PHYSFS_close(file);
        return -1;
    }
    PHYSFS_sint64 size = PHYSFS_fileLength(file);
    if (PHYSFS_readBytes(file, buffer, size) < size)
    {
        ENGINE_LOG("Physfs fails with error: %s", PHYSFS_getLastErrorCode());
        PHYSFS_close(file);
        return -1;
    }
    return size;
    PHYSFS_close(file);
}

unsigned int ModuleFileSystem::Save(const char* filePath, const void* buffer, unsigned int size, bool append) const
{
    PHYSFS_File* file = append ? PHYSFS_openAppend(filePath) : PHYSFS_openWrite(filePath);
    if (file == NULL)
    {
        ENGINE_LOG("Physfs fails with error: %s", PHYSFS_getLastErrorCode());
        PHYSFS_close(file);
        return 1;
    }
    if (PHYSFS_writeBytes(file, buffer, size) < size)
    {
        ENGINE_LOG("Physfs fails with error: %s", PHYSFS_getLastErrorCode());
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
        ENGINE_LOG("Physfs fails with error: %s", PHYSFS_getLastErrorCode());
        return false;
    }
    return true;
}

bool ModuleFileSystem::CleanUp() {
    PHYSFS_deinit();
    return true;
}

