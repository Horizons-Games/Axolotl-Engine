#pragma once

#include "Component.h"
#include "ModuleRender.h"

class ComponentRender : public Component
{
public:
	ComponentRender(bool active, GameObject* owner);
	~ComponentRender() override;

	void SetBloomIntensity(float intensity);
	float GetBloomIntensity() const;

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	ModuleRender* moduleRender;
};

inline void ComponentRender::SetBloomIntensity(float intensity)
{
	moduleRender->SetBloomIntensity(intensity);
}

inline float ComponentRender::GetBloomIntensity() const
{
	return moduleRender->GetBloomIntensity();
}
