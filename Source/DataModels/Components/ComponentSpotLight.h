#pragma once

#include "ComponentLight.h"

class ComponentSpotLight : public ComponentLight
{
public:
	ComponentSpotLight();
	ComponentSpotLight(const float3 &position, const float3& aim, float radius, 
		               int innerAngle, int outerAntgle, const float3& color, float intensity);
	~ComponentSpotLight() {};

	const float3& GetPosition() const;
	const float3& GetAim() const;
	float GetRadius() const;
	float GetInnerAngle() const;
	float GetOuterAngle() const;

	void SetPosition(const float3& position);
	void SetAim(const float3& aim);
	void SetRadius(float radius);
	void SetInnerAngle(int angle);
	void SetOuterAngle(int angle);

private:
	float3 position = float3(0.0f, 0.0f, 0.0f);
	float3 aim = float3(0.0f, 0.0f, 0.0f);
	float radius = 1.0f;
	int innerAngle = 25;
	int outerAngle = 30;
};

inline const float3& ComponentSpotLight::GetPosition() const
{
	return position;
}

inline const float3& ComponentSpotLight::GetAim() const
{
	return aim;
}

inline float ComponentSpotLight::GetRadius() const
{
	return radius;
}

inline float ComponentSpotLight::GetInnerAngle() const
{
	return innerAngle;
}

inline float ComponentSpotLight::GetOuterAngle() const
{
	return outerAngle;
}

inline void ComponentSpotLight::SetPosition(const float3& position)
{
	this->position = position;
}

inline void ComponentSpotLight::SetAim(const float3& aim)
{
	this->aim = aim;
}

inline void ComponentSpotLight::SetRadius(float radius)
{
	this->radius = radius;
}

inline void ComponentSpotLight::SetInnerAngle(int angle)
{
	innerAngle = angle;
}

inline void ComponentSpotLight::SetOuterAngle(int angle)
{
	outerAngle = angle;
}
