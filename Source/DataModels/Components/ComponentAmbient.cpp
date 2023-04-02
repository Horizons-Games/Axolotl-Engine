#include "ComponentAmbient.h"

#include "../Modules/ModuleScene.h"

ComponentAmbient::ComponentAmbient() : ComponentLight(LightType::AMBIENT, false)
{
}

ComponentAmbient::ComponentAmbient(GameObject* parent) : ComponentLight(LightType::AMBIENT, parent, false)
{
}

ComponentAmbient::ComponentAmbient(const float3& color) : ComponentLight(LightType::AMBIENT, color, 1.0f, false)
{
}

ComponentAmbient::ComponentAmbient(const float3& color, GameObject* parent) :
	ComponentLight(LightType::AMBIENT, color, 1.0f, parent, false)
{
}

ComponentAmbient::~ComponentAmbient()
{
}

void ComponentAmbient::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool) active;
	meta["removed"] = (bool) canBeRemoved;

	meta["color_light_X"] = (float) color.x;
	meta["color_light_Y"] = (float) color.y;
	meta["color_light_Z"] = (float) color.z;

	meta["intensity"] = (float) intensity;

	meta["lightType"] = GetNameByLightType(lightType).c_str();
}

void ComponentAmbient::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool) meta["active"];
	canBeRemoved = (bool) meta["removed"];

	color.x = (float) meta["color_light_X"];
	color.y = (float) meta["color_light_Y"];
	color.z = (float) meta["color_light_Z"];

	intensity = (float) meta["intensity"];

	lightType = GetLightTypeByName(meta["lightType"]);
}