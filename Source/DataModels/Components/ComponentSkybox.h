#pragma once
#include "Components/Component.h"

class Skybox;
class Cubemap;

class ComponentSkybox : public Component
{
public:
	ComponentSkybox(bool active, GameObject* owner);
	~ComponentSkybox() override;

	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	void SignalEnable() override;
	void SignalDisable() override;

	void SetSkybox(Skybox* skybox);
	Skybox* GetSkybox();
	Cubemap* GetCubemap();

	bool GetUseCubeMap();
	void SetUseCubeMap(bool use);

private:
	Skybox* skybox;
	Cubemap* cubemap;

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
