#include "WindowComponentSlider.h"

#include "Components/UI/ComponentSlider.h"

#include "Application.h"

WindowComponentSlider::WindowComponentSlider(ComponentSlider* component) : ComponentWindow("SLIDER", component)
{
}

WindowComponentSlider::~WindowComponentSlider()
{
}

void WindowComponentSlider::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text("");
	ComponentSlider* asSlider = static_cast<ComponentSlider*>(component);

	if (asSlider)
	{
		float currentValue = asSlider->GetCurrentValue();
		if (ImGui::SliderFloat("Value", &currentValue, asSlider->GetMinValue(), asSlider->GetMinValue()))
		{
			asSlider->ModifyCurrentValue(currentValue);
		}
	}
}
