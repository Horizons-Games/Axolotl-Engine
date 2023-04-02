#include "WindowComponentCamera.h"

#include "Camera/CameraGameObject.h"
#include "DataModels/Components/ComponentCamera.h"

WindowComponentCamera::WindowComponentCamera(ComponentCamera* component) : ComponentWindow("CAMERA", component)
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
		ImGui::Text("");

		const char* listbox_items[] = { "Basic Frustum", "Offset Frustum", "No Frustum" };

		bool drawFrustum = asCamera->GetCamera()->IsDrawFrustum();
		EFrustumMode frustumMode = asCamera->GetCamera()->GetFrustumMode();
		int frustumModeAsNumber = static_cast<int>(frustumMode);
		float frustumOffset = asCamera->GetCamera()->GetFrustumOffset();

		ImGui::Text("Draw Frustum");
		ImGui::SameLine();
		ImGui::Checkbox("##Draw Frustum", &drawFrustum);

		ImGui::ListBox(
			"Frustum Mode\n(single select)", &frustumModeAsNumber, listbox_items, IM_ARRAYSIZE(listbox_items), 3);
		ImGui::SliderFloat("Frustum Offset", &frustumOffset, -2.f, 2.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);

		ImGui::Separator();

		asCamera->GetCamera()->SetIsDrawFrustum(drawFrustum);
		EFrustumMode newFrustumMode = static_cast<EFrustumMode>(frustumModeAsNumber);
		asCamera->GetCamera()->SetFrustumMode(newFrustumMode);
		asCamera->GetCamera()->SetFrustumOffset(frustumOffset);
	}
}
