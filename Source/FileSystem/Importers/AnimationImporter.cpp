#include "AnimationImporter.h"

#include "Application.h"

#include "FileSystem/ModuleFileSystem.h"

void AnimationImporter::Import(const char* filePath, std::shared_ptr<ResourceAnimation> resource)
{
	char* loadBuffer;
	char* saveBuffer;
	unsigned int size;

	App->fileSystem->Load(filePath, loadBuffer);

	Save(resource, saveBuffer, size);
	App->fileSystem->Save((resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), saveBuffer, size);

	delete loadBuffer;
	delete saveBuffer;
}

void AnimationImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceAnimation> resource)
{
	unsigned int header[4];
	memcpy(header, fileBuffer, sizeof(header));
}

void AnimationImporter::Save(const std::shared_ptr<ResourceAnimation>& resource, char*& fileBuffer, unsigned int& size)
{
	
}
