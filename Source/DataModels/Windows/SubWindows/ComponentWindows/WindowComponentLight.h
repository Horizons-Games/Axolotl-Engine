#pragma once

#include "ComponentWindow.h"

class ComponentLight;

class WindowComponentLight : public ComponentWindow
{
public:
	WindowComponentLight(ComponentLight* component);
	WindowComponentLight(const std::string& name, ComponentLight* component);
	~WindowComponentLight() override;

protected:
	void DrawWindowContents() override;

	constexpr static float max_intensity = 5000.0f;
};

inline void WindowComponentLight::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ImGui::Text("This is a basic light");
}
