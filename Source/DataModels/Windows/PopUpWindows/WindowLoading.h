#pragma once

#include "DataModels/Windows/PopUpWindows/PopUpWindow.h"
#include "imgui.h"

class WindowLoading : public PopUpWindow
{
public:
	WindowLoading();
	~WindowLoading() override;

	void AddWaitingOn(const std::string& waitingCondition, std::optional<float> percentage = std::nullopt);
	void ResetWaitingOn();

protected:
	void DrawWindowContents() override;

private:
	const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
	bool DrawSpinner(const char* label, float radius, int thickness, const ImU32& color);
	
	using WaitingOnData = std::pair<std::string, std::optional<float>>;
	std::vector<WaitingOnData> waitingOn;
};

inline void WindowLoading::AddWaitingOn(const std::string& waitingCondition, std::optional<float> percentage)
{
	waitingOn.push_back(std::make_pair(waitingCondition, percentage));
}

inline void WindowLoading::ResetWaitingOn()
{
	waitingOn.clear();
}
