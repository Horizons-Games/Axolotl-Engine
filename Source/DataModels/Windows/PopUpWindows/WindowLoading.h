#pragma once

#include "DataModels/Windows/PopUpWindows/PopUpWindow.h"
#include "imgui.h"

#include <vector>

class WindowLoading : public PopUpWindow
{
public:
	WindowLoading();
	~WindowLoading() override;

	void AddWaitingOn(const std::string& waitingCondition);
	void ResetWaitingOn();

protected:
	void DrawWindowContents() override;

private:
	const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
	bool DrawSpinner(const char* label, float radius, int thickness, const ImU32& color);
	std::vector<std::string> waitingOn;
};

inline void WindowLoading::AddWaitingOn(const std::string& waitingCondition)
{
	waitingOn.push_back(waitingCondition);
}

inline void WindowLoading::ResetWaitingOn()
{
	waitingOn.clear();
}
