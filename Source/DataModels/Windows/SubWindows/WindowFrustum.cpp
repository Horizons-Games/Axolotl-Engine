#include "WindowFrustum.h"

#include "imgui.h"
#include "Application.h"
#include "Modules/ModuleDebugDraw.h"
#include "Modules/ModuleEngineCamera.h"

WindowFrustum::WindowFrustum() : SubWindow("Frustum")
{
}

WindowFrustum::~WindowFrustum()
{
}

void WindowFrustum::DrawWindowContents()
{
	bool showAABBs = App->debug->IsShowingBoundingBoxes();
	if (ImGui::Checkbox("Show bounding boxes", &showAABBs))
	{
		App->debug->ShowBoundingBoxes(showAABBs);
	}
	const char* listbox_items[] = { "Basic Frustum", "Offset Frustum", "No Frustum"};

	int currentFrustum = App->engineCamera->GetFrustumMode();
	if (ImGui::ListBox("Frustum Mode\n(single select)", &currentFrustum, listbox_items, IM_ARRAYSIZE(listbox_items), 3))
	{
		App->engineCamera->SetFrustumMode(currentFrustum);
	}

	float vFrustum = App->engineCamera->GetFrustumOffset();
	if (ImGui::SliderFloat("Offset", &vFrustum, MIN_FRUSTUM, MAX_FRUSTUM, "%.0f", ImGuiSliderFlags_AlwaysClamp)) {
		App->engineCamera->SetFrustumOffset(vFrustum);
	}

}

