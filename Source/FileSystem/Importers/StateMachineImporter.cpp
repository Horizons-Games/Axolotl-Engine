#pragma warning (disable: 6386)

#include "StateMachineImporter.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"

StateMachineImporter::StateMachineImporter()
{
}

StateMachineImporter::~StateMachineImporter()
{
}

void StateMachineImporter::Import(const char* filePath, std::shared_ptr<ResourceStateMachine> resource)
{
}

void StateMachineImporter::Save(const std::shared_ptr<ResourceStateMachine>& resource, char*& fileBuffer, unsigned int& size)
{
}

void StateMachineImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceStateMachine> resource)
{
}