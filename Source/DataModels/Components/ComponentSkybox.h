#pragma once
#include "Components/Component.h"
#include "FileSystem/UID.h"

class ResourceSkyBox;

class ComponentSkybox : public Component
{
public:
	ComponentSkybox(bool active, GameObject* owner);
	~ComponentSkybox() override;
	
	
	void Draw(float4x4 view, float4x4 proj) const;
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