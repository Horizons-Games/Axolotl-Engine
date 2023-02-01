#include "WindowComponentCamera.h"

#include "DataModels/Components/ComponentCamera.h"

WindowComponentCamera::WindowComponentCamera(const std::weak_ptr<ComponentCamera>& component) :
	ComponentWindow("CAMERA", component)
{
}

void WindowComponentCamera::DrawWindowContents()
{
	this->DrawEnableAndDeleteComponent();

	std::shared_ptr<ComponentCamera> asSharedCamera =
		std::dynamic_pointer_cast<ComponentCamera>(this->component.lock());

	if (asSharedCamera)
	{
		const char* listbox_items[] = { "Basic Frustum", "Offset Frustum", "No Frustum" };

		bool drawFrustum = asSharedCamera->IsDrawFrustum();
		int frustumMode = asSharedCamera->GetFrustumMode();
		float frustumOffset = asSharedCamera->GetFrustumOffset();

		ImGui::Text("Draw Frustum"); ImGui::SameLine();
		ImGui::Checkbox("##Draw Frustum", &drawFrustum);

		ImGui::ListBox("Frustum Mode\n(single select)", &frustumMode, listbox_items, IM_ARRAYSIZE(listbox_items), 3);
		ImGui::SliderFloat("Frustum Offset", &frustumOffset, -2.f, 2.f, "%.0f", ImGuiSliderFlags_AlwaysClamp);

		ImGui::Separator();

		asSharedCamera->SetDrawFrustum(drawFrustum);
		asSharedCamera->SetFrustumMode(frustumMode);
		asSharedCamera->SetFrustumOffset(frustumOffset);
	}
}
