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

#include <iostream>
#include <filesystem>


VideoImporter::VideoImporter()
{
}

VideoImporter::~VideoImporter()
{
}

void VideoImporter::Import(const char* filePath, std::shared_ptr<ResourceVideo> resource)
{
	std::filesystem::path fs_path(resource->GetAssetsPath());
	resource->SetExtension(fs_path.extension().string());
	App->GetModule<ModuleFileSystem>()->Copy(resource->GetAssetsPath(), resource->GetLibraryPath() + resource->GetExtension());
}

void VideoImporter::Load(const char* fileBuffer, std::shared_ptr<ResourceVideo> resource)
{
}

void VideoImporter::Save(const std::shared_ptr<ResourceVideo>& resource, char*& fileBuffer, unsigned int& size)
{
}