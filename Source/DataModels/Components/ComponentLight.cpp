#include "ComponentLight.h"

ComponentLight::ComponentLight(const bool active, GameObject* owner)
	: Component(ComponentType::LIGHT, active, owner, true),
	color(float3(1.0f, 1.0f, 1.0f)), intensity(1.0f)
{
}

ComponentLight::ComponentLight(LightType type, bool canBeRemoved) 
	: Component(ComponentType::LIGHT, true, nullptr, canBeRemoved),
	color(float3(1.0f, 1.0f, 1.0f)), intensity(1.0f), lightType(type)
{
};

ComponentLight::ComponentLight(LightType type, GameObject* gameObject, bool canBeRemoved) 
	: Component(ComponentType::LIGHT, true, gameObject, canBeRemoved),
	color(float3(1.0f, 1.0f, 1.0f)), intensity(1.0f), lightType(type)
{
}

ComponentLight::ComponentLight(LightType type, const float3& color, float intensity, bool canBeRemoved) 
	: Component(ComponentType::LIGHT, true, nullptr, canBeRemoved),
	color(color), intensity(intensity), lightType(type)
{
}

ComponentLight::ComponentLight(LightType type, const float3& color, float intensity,
								GameObject* gameObject, bool canBeRemoved) :
	Component(ComponentType::LIGHT, true, gameObject, canBeRemoved),
	color(color), intensity(intensity), lightType(type)
{
}

ComponentLight::~ComponentLight()
{
}

const std::string GetNameByLightType(LightType type)
{
	switch (type)
	{
	case LightType::DIRECTIONAL:
		return "LightType_Directional";
	case LightType::POINT:
		return "LightType_Point";
	case LightType::SPOT:
		return "LightType_Spot";
	case LightType::AMBIENT:
		return "LightType_Ambient";
	default:
		assert(false && "Wrong light type introduced");
		return "";
	}
}

const LightType GetLightTypeByName(const std::string& typeName)
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

	if (typeName == "LightType_Ambient")
	{
		return LightType::AMBIENT;
	}
	return LightType::UNKNOWN;
}