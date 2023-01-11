#include "ComponentAmbient.h"

ComponentAmbient::ComponentAmbient() : ComponentLight(LightType::AMBIENT)
{
}

ComponentAmbient::ComponentAmbient(const float3& color) : ComponentLight(LightType::AMBIENT, color, 1.0f)
{
}

