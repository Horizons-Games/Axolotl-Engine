#pragma once

#include "ComponentLight.h"

struct AreaLightSphere
{
	float4 position; // xyz position+w radius
	float4 color;	 // rgb colour+alpha intensity
	float attRadius; // radius for attenuation
	float padding1;
	float2 padding2;
};

struct AreaLightTube
{
	float4 positionA;
	float4 positionB;
	float4 color;	 // rgb colour+alpha intensity
	float attRadius; // radius for attenuation
	float padding3;
	float2 padding4;
};

class ComponentAreaLight : public ComponentLight
{
public:
	ComponentAreaLight();
	ComponentAreaLight(const ComponentAreaLight& componentAreaLight);
	ComponentAreaLight(AreaType areaType, GameObject* parent);
	ComponentAreaLight(const float3& color, float intensity, AreaType areaType);
	ComponentAreaLight(const float3& color, float intensity, GameObject* parent, AreaType areaType);
	~ComponentAreaLight() override;

	void Draw() const override;

	const AreaType GetAreaType();
	const float GetShapeRadius();
	const float GetHeight();
	const float GetAttRadius();

	void SetAreaType(AreaType newType);
	void SetShapeRadius(float newRadius);
	void SetHeight(float newHeight);
	void SetLightRadius(float newRadius);

	void OnTransformChanged();

private:
	void SignalEnable() override;
	void SignalDisable() override;

	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

private:
	const std::string GetNameByAreaType(AreaType type);
	const AreaType GetAreaTypeByName(const std::string& typeName);

	AreaType areaType;
	float shapeRadius;
	float height;
	float attRadius;
};

inline const AreaType ComponentAreaLight::GetAreaType()
{
	return areaType;
}

inline const float ComponentAreaLight::GetShapeRadius()
{
	return shapeRadius;
}

inline const float ComponentAreaLight::GetHeight()
{
	return height;
}

inline const float ComponentAreaLight::GetAttRadius()
{
	return attRadius;
}

inline void ComponentAreaLight::SetAreaType(AreaType newType)
{
	areaType = newType;
}

inline void ComponentAreaLight::SetShapeRadius(float newRadius)
{
	shapeRadius = newRadius;
}

inline void ComponentAreaLight::SetHeight(float newHeight)
{
	height = newHeight;
}

inline void ComponentAreaLight::SetLightRadius(float newRadius)
{
	attRadius = newRadius;
}

inline const std::string ComponentAreaLight::GetNameByAreaType(AreaType type)
{
	switch (type)
	{
		case AreaType::QUAD:
			return "AreaType_Quad";
		case AreaType::TUBE:
			return "AreaType_Tube";
		case AreaType::SPHERE:
			return "AreaType_Sphere";
		case AreaType::DISK:
			return "AreaType_Disc";
		default:
			assert(false && "Wrong area type introduced");
			return "";
	}
}

inline const AreaType ComponentAreaLight::GetAreaTypeByName(const std::string& typeName)
{
	if (typeName == "AreaType_Quad")
	{
		return AreaType::QUAD;
	}

	if (typeName == "AreaType_Tube")
	{
		return AreaType::TUBE;
	}

	if (typeName == "AreaType_Sphere")
	{
		return AreaType::SPHERE;
	}

	if (typeName == "AreaType_Disc")
	{
		return AreaType::DISK;
	}
	return AreaType::NONE;
}
