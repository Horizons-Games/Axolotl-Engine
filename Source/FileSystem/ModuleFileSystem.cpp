#include "ModuleFileSystem.h"
#include <fstream>
#include <vector>
#include <cstring>

unsigned int ModuleFileSystem::Load(const char* file_path, char** buffer) const 
{
    std::ifstream file(file_path);
    std::vector<char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    data.push_back('\0');

    char** p = new char* [data.size()];

    std::memcpy(p, data.data(), data.size());
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
