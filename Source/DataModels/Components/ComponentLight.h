#pragma once
#pragma warning (disable: 26495)

#include "Component.h"

#include "GameObject/GameObject.h"

#include "Math/float3.h"

#define COMPONENT_LIGHT "Light"

enum class LightType { UNKNOWN, DIRECTIONAL, POINT, SPOT, AMBIENT };

const static std::string GetNameByLightType(LightType type);
const static LightType GetLightTypeByName(const std::string& name);

class Json;

class ComponentLight : public Component
{
public:
	ComponentLight(const bool active, GameObject* owner);
	ComponentLight(LightType type, bool canBeRemoved);
	ComponentLight(LightType type, GameObject* gameObject, bool canBeRemoved);
	ComponentLight(LightType type, const float3& color, float intensity, bool canBeRemoved);
	ComponentLight(LightType type, const float3& color, float intensity, 
					GameObject* gameObject, bool canBeRemoved);

	virtual ~ComponentLight();

	void Update() override;

	void Enable() override;
	void Disable() override;

	virtual void Draw() override {};

	virtual void SaveOptions(Json& meta) override {};
	virtual void LoadOptions(Json& meta) override {};

	const float3& GetColor() const;
	float GetIntensity() const;
	LightType GetLightType() const;

	void SetColor(const float3& color);
	void SetIntensity(float intensity);

protected:
	float3 color;
	float intensity;

	LightType lightType;
};

inline void ComponentLight::Update()
{
	Draw();
}

inline void ComponentLight::Enable()
{
	Component::Enable();
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

inline LightType ComponentLight::GetLightType() const
{
	return lightType;
}

inline void ComponentLight::SetColor(const float3& color)
{
	this->color = color;
}

inline void ComponentLight::SetIntensity(float intensity)
{
	this->intensity = intensity;
}