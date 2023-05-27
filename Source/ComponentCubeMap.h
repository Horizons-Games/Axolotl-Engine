#pragma once
#include "Components/Component.h"


class ComponentCubeMap : public Component
{
public:
	ComponentCubeMap(bool active, GameObject* owner);
	~ComponentCubeMap() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	bool IsStatic();
	void SetStatic(bool newStatic);
	
	void SetIntensity(float intensity);
	float GetIntensity();

private:
	float intensity;
};

inline float ComponentCubeMap::GetIntensity()
{
	return intensity;
}
