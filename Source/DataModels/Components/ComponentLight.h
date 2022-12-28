#pragma once

#include "Component.h"

#include "Math/float3.h"

enum class LightType { DIRECTIONAL, POINT, SPOT, AMBIENT };

class ComponentLight : public Component
{
public:
	ComponentLight(LightType type, const float3& color, float intensity);
	~ComponentLight() {};

	void Enable() override;
	void Update() override;
	void Disable() override;

	const float3& GetDir();
	const float3& GetColor();
	float GetIntensity();
	LightType GetType();

	void SetColor(const float3& color);
	void SetIntensity(float intensity);

private:
	float3 color = float3(1.0f, 1.0f, 1.0f);
	float intensity = 1.f;

	LightType type;
};

inline void ComponentLight::Enable()
{
	Component::Enable();
}

inline void ComponentLight::Update()
{
	Component::Update();
}

inline void ComponentLight::Disable()
{
	Component::Disable();
}

inline const float3& ComponentLight::GetColor()
{
	return color;
}

inline float ComponentLight::GetIntensity()
{
	return intensity;
}

inline LightType ComponentLight::GetType()
{
	return type;
}