#include "WindowComponentLight.h"

#include "DataModels/Components/ComponentLight.h"

WindowComponentLight::WindowComponentLight(ComponentLight* component) : ComponentWindow("Basic light", component)
{
}

WindowComponentLight::WindowComponentLight(const std::string& name, ComponentLight* component) :
	ComponentWindow(name, component)
{
}

WindowComponentLight::~WindowComponentLight()
{
}
