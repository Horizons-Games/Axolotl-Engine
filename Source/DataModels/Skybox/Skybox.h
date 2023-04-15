#pragma once

#include "FileSystem/UniqueID.h"

#include <memory>

class ResourceSkyBox;
class Json;

class Skybox
{

public:
	Skybox();
	Skybox(const std::shared_ptr<ResourceSkyBox>& skyboxRes);
	~Skybox();

	void Draw() const;
	void SaveOptions(Json& json) const;
	void LoadOptions(Json& json);

private:
	std::shared_ptr<ResourceSkyBox> skyboxRes;
};