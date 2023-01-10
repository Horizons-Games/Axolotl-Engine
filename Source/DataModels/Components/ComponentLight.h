#pragma once

#include "Component.h"

#include "Math/float3.h"

enum class LightType { DIRECTIONAL, POINT, SPOT, AMBIENT };

class ComponentLight : public Component
{
public:
	ComponentLight(LightType type);
	ComponentLight(LightType type, const float3& color, float intensity);
	~ComponentLight() {};

	void Init() override;
	void Update() override;

	void Enable() override;
	void Display() override;
	void Disable() override;

	const float3& GetColor() const;
	float GetIntensity() const;
	LightType GetType() const;

	void SetColor(const float3& color);
	void SetIntensity(float intensity);

private:
	float3 color = float3(1.0f, 1.0f, 1.0f);
	float intensity = 1.f;

	LightType type;
};

inline void ComponentLight::Init()
{
}

inline void ComponentLight::Update()
{
}

<<<<<<< HEAD
inline void ComponentLight::Enable()
{
	Component::Enable();
=======
inline void ComponentLight::Display()
{
>>>>>>> origin/scenemanagement
}

inline void ComponentLight::Disable()
{
	Component::Disable();
}

inline const float3& ComponentLight::GetColor() const
{
	return color;
}

inline float ComponentLight::GetIntensity() const
{
	return intensity;
}

inline LightType ComponentLight::GetType() const
{
	return type;
}

inline void ComponentLight::SetColor(const float3& color)
{
	this->color = color;
}

inline void ComponentLight::SetIntensity(float intensity)
{
	this->intensity = intensity;
}