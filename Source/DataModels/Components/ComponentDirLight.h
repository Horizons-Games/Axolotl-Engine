#pragma once

#include "ComponentLight.h"

class ComponentDirLight : public ComponentLight
{
public:
	ComponentDirLight(const float3 &dir, const float3& color, float intensity);
	~ComponentDirLight() {};

	const float3& GetDir();

	void SetDir(const float3& dir);

private:
	float3 dir = float3(0.0f, 0.0f, 1.0f);
};

inline const float3& ComponentDirLight::GetDir()
{
	return dir;
}
