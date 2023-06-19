#include "ResourceScene.h"

ResourceScene::ResourceScene(UID resourceUID,
							 const std::string& fileName,
							 const std::string& assetsPath,
							 const std::string& libraryPath) :
	Resource(resourceUID, fileName, assetsPath, libraryPath)
{
}

ResourceScene::~ResourceScene()
{
	this->Unload();
}