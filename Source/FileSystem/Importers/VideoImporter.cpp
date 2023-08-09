#include "StdAfx.h"

#include "VideoImporter.h"

#include "Application.h"
#include "FileSystem/Json.h"
#include "FileSystem/ModuleFileSystem.h"
#include "Resources/ResourceVideo.h"
#include "AxoLog.h"

#include "Defines/ExtensionDefines.h"

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libswresample/swresample.h>
}


VideoImporter::VideoImporter()
{
}

VideoImporter::~VideoImporter()
{
}

void VideoImporter::Import(const char* filePath, std::shared_ptr<ResourceVideo> resource)
{
	char* buffer{};
	unsigned int size = 0;
	Save(resource, buffer, size);
	App->GetModule<ModuleFileSystem>()->Save(
		(resource->GetLibraryPath() + GENERAL_BINARY_EXTENSION).c_str(), buffer, 0);
	delete buffer;
}

void VideoImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceVideo> resource)
{
}

void VideoImporter::Save(const std::shared_ptr<ResourceVideo>& resource, char*& fileBuffer, unsigned int& size)
{
}