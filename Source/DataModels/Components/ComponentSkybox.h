#pragma once
#include "Components/Component.h"
#include "FileSystem/UID.h"

class ResourceSkyBox;
class Cubemap;

class ComponentSkybox : public Component
{
public:
	ComponentSkybox(bool active, GameObject* owner);
	~ComponentSkybox() override;
	
	
	void Draw(float4x4 view, float4x4 proj) const;
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	std::shared_ptr<ResourceSkyBox> GetSkyboxResource() const;
	void SetSkyboxResource(std::shared_ptr<ResourceSkyBox> resource);
	Cubemap* GetCubemap();

	bool GetUseCubeMap();
	void SetUseCubeMap(bool use);

private:
	std::shared_ptr<ResourceSkyBox> skyboxRes;

	std::unique_ptr<Cubemap> cubemap;

	bool useCubemap;
};

inline bool ComponentSkybox::GetUseCubeMap()
{
	return useCubemap;
}

inline void ComponentSkybox::SetUseCubeMap(bool use)
{
	useCubemap = use;
}
