#pragma once
#include "Components/Component.h"
#include "Auxiliar/Generics/Drawable.h"
#include "Auxiliar/Generics/Updatable.h"
#include "UID.h"

class ResourceSkyBox;
class Skybox;


class ComponentSkybox : public Component, public Drawable
{
public:
	ComponentSkybox(bool active, GameObject* owner);
	~ComponentSkybox() override;
	
	
	void Draw() const override;
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	void SignalEnable() override;
	void SignalDisable() override;

	std::shared_ptr<ResourceSkyBox> GetSkyboxResource() const;
	void SetSkyboxResource(std::shared_ptr<ResourceSkyBox> resource);

private:
	std::shared_ptr<ResourceSkyBox> skyboxRes;
	bool enable;
};