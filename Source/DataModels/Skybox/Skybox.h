#pragma once

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
	std::shared_ptr<ResourceSkyBox> GetSkyboxResource() const;

private:
	std::shared_ptr<ResourceSkyBox> skyboxRes;
};