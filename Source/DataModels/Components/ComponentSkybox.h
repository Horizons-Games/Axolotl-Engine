#pragma once
#include "Components/Component.h"
#include "Skybox/Skybox.h"

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

private:
	Skybox* skybox;
};