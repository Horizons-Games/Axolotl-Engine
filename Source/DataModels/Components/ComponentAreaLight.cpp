#include "ComponentAreaLight.h"

#include "FileSystem/Json.h"

ComponentAreaLight::ComponentAreaLight() : ComponentLight(LightType::AREA, true), areaType(AreaType::SPHERE), shapeRadius(1.f), 
	attRadius(2.f), height(1.f)
{
}

ComponentAreaLight::ComponentAreaLight(const ComponentAreaLight& componentAreaLight) :
	ComponentLight(componentAreaLight), areaType(componentAreaLight.areaType), shapeRadius(componentAreaLight.shapeRadius),
	attRadius(componentAreaLight.attRadius), height(componentAreaLight.height)
{
}

ComponentAreaLight::ComponentAreaLight(AreaType areaType, GameObject* parent) :
	ComponentLight(LightType::AREA, parent, true), areaType(areaType), shapeRadius(1.f), attRadius(2.f), height(1.f)
{	
}

ComponentAreaLight::ComponentAreaLight(const float3& color, float intensity, AreaType areaType) :
	ComponentLight(LightType::AREA, color, intensity, true), areaType(areaType), shapeRadius(1.f), attRadius(2.f), height(1.f)
{
}

ComponentAreaLight::ComponentAreaLight(const float3& color, float intensity, GameObject* parent, AreaType areaType) :
	ComponentLight(LightType::AREA, color, intensity, parent, true), areaType(areaType), shapeRadius(1.f), attRadius(2.f), 
	height(1.f)
{
}

ComponentAreaLight::~ComponentAreaLight()
{
}

void ComponentAreaLight::SaveOptions(Json& meta)
{
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;

	meta["color_light_X"] = (float)color.x;
	meta["color_light_Y"] = (float)color.y;
	meta["color_light_Z"] = (float)color.z;

	meta["intensity"] = (float)intensity;

	meta["lightType"] = GetNameByLightType(lightType).c_str();
	
	meta["areaType"] = GetNameByAreaType(areaType).c_str();
	meta["shapeRadius"] = (float)shapeRadius;
	meta["height"] = (float)height;
	meta["attRadius"] = (float)attRadius;
}

void ComponentAreaLight::LoadOptions(Json& meta)
{
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];

	color.x = (float)meta["color_light_X"];
	color.y = (float)meta["color_light_Y"];
	color.z = (float)meta["color_light_Z"];

	intensity = (float)meta["intensity"];

	lightType = GetLightTypeByName(meta["lightType"]);

	areaType = GetAreaTypeByName(meta["areaType"]);
	shapeRadius = (float)meta["shapeRadius"];
	height = (float)meta["height"];
	attRadius = (float)meta["attRadius"];
}
