#include "WindowLoading.h"
#include "imgui_internal.h"

WindowLoading::WindowLoading() : EditorWindow("Loading")
{
	flags |= ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoMove
        | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
}

WindowLoading::~WindowLoading()
{
}

void WindowLoading::AddWaitingOn(const std::string& waitingCondition)
{
    waitingOn.push_back(waitingCondition);
}

void WindowLoading::ResetWaitingOn()
{
    waitingOn.clear();
}

void WindowLoading::DrawWindowContents()
{
	ImGui::SetWindowPos(ImGui::GetWindowViewport()->GetCenter());
	ImGui::SetWindowCollapsed(false);
	ImGui::SetWindowFocus();
	
	if (waitingOn.empty())
    {
	    ImGui::TextUnformatted("Loading...");
		DrawSpinner("##spinner", 15, 6, col);
        return;
    }

    for (const std::string& waitingCondition : waitingOn)
    {
		ImGui::TextUnformatted(waitingCondition.c_str());
		DrawSpinner(("##spinner" + waitingCondition).c_str(), 15, 6, col);
    }
}

//from: https://github.com/ocornut/imgui/issues/1901
bool WindowLoading::DrawSpinner(const char* label, float radius, int thickness, const ImU32& color) {
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label);

    ImVec2 pos = window->DC.CursorPos;
    ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

    const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
    ImGui::ItemSize(bb, style.FramePadding.y);
    if (!ImGui::ItemAdd(bb, id))
        return false;

    // Render
    window->DrawList->PathClear();

    int num_segments = 30;
    int start = abs(ImSin(g.Time * 1.8f) * (num_segments - 5));

    const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
    const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

    const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

    for (int i = 0; i < num_segments; i++) {
        const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
        window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + g.Time * 8) * radius,
            centre.y + ImSin(a + g.Time * 8) * radius));
    }

    window->DrawList->PathStroke(color, false, thickness);
}
