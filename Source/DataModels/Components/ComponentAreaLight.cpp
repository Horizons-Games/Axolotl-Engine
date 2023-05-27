#include "ComponentAreaLight.h"

#include "ComponentMeshRenderer.h"
#include "Resources/ResourceMesh.h"
#include "Resources/ResourceMaterial.h"
#include "Application.h"
#include "ModuleProgram.h"

#include "Math/float3.h"
#include <vector>
#include <memory>

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
