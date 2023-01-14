#include "ComponentAmbient.h"

ComponentAmbient::ComponentAmbient() : ComponentLight(LightType::AMBIENT)
{
}

ComponentAmbient::ComponentAmbient(GameObject* parent) : ComponentLight(LightType::AMBIENT, parent)
{
}

ComponentAmbient::ComponentAmbient(const float3& color) : ComponentLight(LightType::AMBIENT, color, 1.0f)
{
}

ComponentAmbient::ComponentAmbient(const float3& color, GameObject* parent) : 
	ComponentLight(LightType::AMBIENT, color, 1.0f, parent)
{
}
