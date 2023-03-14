#include "WindowComponentCamera.h"

#include "DataModels/Components/ComponentCamera.h"

WindowComponentCamera::WindowComponentCamera(ComponentCamera* component) :
	ComponentWindow("CAMERA", component)
{
}

WindowComponentCamera::~WindowComponentCamera()
{
}

void WindowComponentCamera::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ComponentCamera* asCamera = static_cast<ComponentCamera*>(component);

	if (asCamera)
	{
		const char* listbox_items[] = { "Basic Frustum", "Offset Frustum", "No Frustum" };

		bool drawFrustum = asCamera->IsDrawFrustum();
		ECameraFrustumMode frustumMode = asCamera->GetFrustumMode();
		int frustumModeAsNumber = static_cast<int>(frustumMode);
		float frustumOffset = asCamera->GetFrustumOffset();

		ImGui::Text("Draw Frustum"); ImGui::SameLine();
		ImGui::Checkbox("##Draw Frustum", &drawFrustum);

		ImGui::ListBox("Frustum Mode\n(single select)", &frustumModeAsNumber, listbox_items, IM_ARRAYSIZE(listbox_items), 3);
		ImGui::SliderFloat("Frustum Offset", &frustumOffset, -2.f, 2.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);

		ImGui::Separator();

		asCamera->SetDrawFrustum(drawFrustum);
		ECameraFrustumMode newFrustumMode = static_cast<ECameraFrustumMode>(frustumModeAsNumber);
		asCamera->SetFrustumMode(newFrustumMode);
		asCamera->SetFrustumOffset(frustumOffset);
	}
}
