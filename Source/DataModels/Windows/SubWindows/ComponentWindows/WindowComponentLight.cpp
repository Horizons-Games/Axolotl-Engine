#include "WindowComponentLight.h"

#include "DataModels/Components/ComponentLight.h"

WindowComponentLight::WindowComponentLight(const std::weak_ptr<ComponentLight>& component) :
	ComponentWindow("BASIC LIGHT", std::weak_ptr<Component>())
{
}

void WindowComponentLight::DrawWindowContents()
{
	this->DrawEnableAndDeleteComponent();

	ImGui::Text("This is a basic light");
}
