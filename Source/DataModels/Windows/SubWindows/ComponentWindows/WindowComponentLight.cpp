#include "WindowComponentLight.h"

#include "DataModels/Components/ComponentLight.h"

WindowComponentLight::WindowComponentLight(ComponentLight* component) : ComponentWindow("BASIC LIGHT", component)
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
