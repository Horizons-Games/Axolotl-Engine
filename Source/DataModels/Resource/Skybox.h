#pragma once

#include "FileSystem/UniqueID.h"

#include <memory>

class ResourceSkyBox;
class Json;

class Skybox
{

public:
	Skybox(const std::weak_ptr<ResourceSkyBox>& skyboxRes);
	void Draw();

	void LoadOptions(Json& meta);

private:
	std::weak_ptr<ResourceSkyBox> skyboxRes;

	UID skyboxUID = 0ULL;


};

