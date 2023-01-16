#pragma once

#include "FileSystem/UniqueID.h"

#include <memory>

class ResourceSkyBox;

class Skybox
{

public:
	Skybox();
	void Draw();

private:
	std::weak_ptr<ResourceSkyBox> skyboxRes;

	UID skyboxUID = 0ULL;
};

