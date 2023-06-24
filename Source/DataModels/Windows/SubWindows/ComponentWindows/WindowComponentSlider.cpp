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
		float minValue = asSlider->GetMinValue();
		float maxValue = asSlider->GetMaxValue();
		if (ImGui::DragFloat("Range", &minValue))
		{
			if (minValue <= maxValue) asSlider->SetMinValue(minValue);
		}
		ImGui::SameLine();
		if (ImGui::DragFloat("##MaxRange", &maxValue))
		{
			if(minValue <= maxValue) asSlider->SetMaxValue(maxValue);
		}

		float currentValue = asSlider->GetCurrentValue();
		if (ImGui::SliderFloat("Value", &currentValue, minValue, maxValue))
		{
			asSlider->ModifyCurrentValue(currentValue);
		}
	}
}
