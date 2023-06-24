#include "StdAfx.h"

#include "DataModels/Windows/PopUpWindows/WindowLoading.h"
#include "imgui_internal.h"

WindowLoading::WindowLoading() : PopUpWindow("Loading")
{
	flags = ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs;
}

WindowLoading::~WindowLoading()
{
}

void WindowLoading::DrawWindowContents()
{
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

// from: https://github.com/ocornut/imgui/issues/1901
bool WindowLoading::DrawSpinner(const char* label, float radius, int thickness, const ImU32& color)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size((radius) *2, (radius + style.FramePadding.y) * 2);

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ImGui::ItemSize(bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
	{
		return false;
	}

	// Render
	window->DrawList->PathClear();

	float num_segments = 30;
	int start = static_cast<int>(abs(ImSin(static_cast<float>(g.Time) * 1.8f) * (num_segments - 5)));

	const float a_min = IM_PI * 2.0f * (static_cast<float>(start)) / num_segments;
	const float a_max = IM_PI * 2.0f * (num_segments - 3) / num_segments;

	const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

	for (float i = 0; i < num_segments; i++)
	{
		const float a = a_min + (i / num_segments) * (a_max - a_min);
		window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a + static_cast<float>(g.Time) * 8) * radius,
											centre.y + ImSin(a + static_cast<float>(g.Time) * 8) * radius));
	}

	window->DrawList->PathStroke(color, false, static_cast<float>(thickness));
	return true;
}
