#pragma once

#include "FileSystem/UniqueID.h"

#include <memory>

class ResourceSkyBox;
class Json;

class Skybox
{

public:
	Skybox(const std::weak_ptr<ResourceSkyBox>& skyboxRes);
	~Skybox();

	void Draw();

private:
	std::weak_ptr<ResourceSkyBox> skyboxRes;

	UID skyboxUID;
};