#pragma once
#include "Auxiliar/Generics/Drawable.h"
#include "Component.h"

#include "GameObject/GameObject.h"

#include "Math/float3.h"

enum class LightType 
{ 
	UNKNOWN, 
	DIRECTIONAL, 
	POINT, 
	SPOT,
	AREA
};

enum class AreaType
{
	SPHERE,
	TUBE,
	QUAD,
	DISC,
	NONE
};

const static std::string GetNameByLightType(LightType type);
const static LightType GetLightTypeByName(const std::string& name);

class Json;

class ComponentLight : public Component, public Drawable
{
public:
	ComponentLight(const bool active, GameObject* owner);
	ComponentLight(LightType type, bool canBeRemoved);
	ComponentLight(LightType type, GameObject* gameObject, bool canBeRemoved);
	ComponentLight(LightType type, const float3& color, float intensity, bool canBeRemoved);
	ComponentLight(LightType type, const float3& color, float intensity, GameObject* gameObject, bool canBeRemoved);
	ComponentLight(const ComponentLight& componentLight);

	virtual ~ComponentLight() override;

	virtual void Draw() const override{};

	void Enable() override;
	void Disable() override;

	virtual void SaveOptions(Json& meta) override{};
	virtual void LoadOptions(Json& meta) override{};

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

inline const std::string GetNameByLightType(LightType type)
{
	switch (type)
	{
		case LightType::DIRECTIONAL:
			return "LightType_Directional";
		case LightType::POINT:
			return "LightType_Point";
		case LightType::SPOT:
			return "LightType_Spot";
		case LightType::AREA:
			return "LightType_Area";
		default:
			assert(false && "Wrong light type introduced");
			return "";
	}
}

inline const LightType GetLightTypeByName(const std::string& typeName)
{
	if (typeName == "LightType_Directional")
	{
		return LightType::DIRECTIONAL;
	}

	if (typeName == "LightType_Point")
	{
		return LightType::POINT;
	}

	if (typeName == "LightType_Spot")
	{
		return LightType::SPOT;
	}

	if (typeName == "LightType_Area")
	{
		return LightType::AREA;
	}
	return LightType::UNKNOWN;
}