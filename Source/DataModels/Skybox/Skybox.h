#pragma once

class ResourceSkyBox;
class Json;

class Skybox
{
public:
	Skybox();
	Skybox(const std::shared_ptr<ResourceSkyBox>& skyboxRes);
	~Skybox();

	void Draw(float4x4 view, float4x4 proj) const;
	void SaveOptions(Json& json) const;
	void LoadOptions(Json& json);

private:
	std::shared_ptr<ResourceSkyBox> skyboxRes;
};