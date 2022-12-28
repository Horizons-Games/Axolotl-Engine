#include "WindowFrustum.h"

#include "imgui.h"
#include "Application.h"
#include "Modules/ModuleDebugDraw.h"

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
}
