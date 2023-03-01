#pragma once

#include "FileSystem/UniqueID.h"

#include <memory>

class ResourceSkyBox;
class Json;

class Skybox
{

public:
	Skybox(const std::shared_ptr<ResourceSkyBox>& skyboxRes);
	void Draw();

private:
	std::shared_ptr<ResourceSkyBox> skyboxRes;

	UID skyboxUID = 0ULL;
};