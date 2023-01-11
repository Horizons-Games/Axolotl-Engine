#pragma once
#include "ComponentLight.h"

class ComponentAmbient : public ComponentLight
{
public:
	ComponentAmbient();
	ComponentAmbient(const float3& color);
	~ComponentAmbient() {};
};

