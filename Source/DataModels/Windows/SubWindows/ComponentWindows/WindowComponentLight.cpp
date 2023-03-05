#include "WindowComponentLight.h"

#include "DataModels/Components/ComponentLight.h"

const float WindowComponentLight::max_intensity = 5000.0f;

WindowComponentLight::WindowComponentLight(ComponentLight* component) :
	ComponentWindow("BASIC LIGHT", component)
{
}

WindowComponentLight::WindowComponentLight(const std::string& name, ComponentLight* component) :
	ComponentWindow(name, component)
{
}

WindowComponentLight::~WindowComponentLight()
{
}

void WindowComponentLight::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ImGui::Text("This is a basic light");
}
