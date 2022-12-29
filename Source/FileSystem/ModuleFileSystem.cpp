#include "ModuleFileSystem.h"
#include <fstream>
#include <vector>
#include <cstring>
#include <direct.h>

unsigned int ModuleFileSystem::Load(const char* filePath, char*& buffer) const
{
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = new char[size];
    if (!file.read(buffer, size))
    {
        return 1;
    }

    // Close the file
    file.close();
    return 0;
}

unsigned int ModuleFileSystem::Save(const char* file_path, const void* buffer, unsigned int size, bool append) const
{
    std::ofstream file(file_path, append ? std::ios::app : std::ios::trunc);
    file.write(static_cast<const char*>(buffer), size);
    return 0;
}

bool ModuleFileSystem::Exists(const char* file_path) const
{
    std::ifstream file(file_path);
    return file.good();
}

bool ModuleFileSystem::IsDirectory(const char* directoryPath) const
{
    struct _stat statbuf;
    int result = _stat(directoryPath, &statbuf);

    if (result == 0 && (statbuf.st_mode & _S_IFDIR)) {
        return true;
    }
    else {
        return false;
    }
}

bool  ModuleFileSystem::CreateDirectory(const char* directoryPath)
{
    if (_mkdir(directoryPath) == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}
