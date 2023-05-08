#pragma once

#include "EditorWindow.h"

class WindowLoading : public EditorWindow
{
public:
	WindowLoading();
	~WindowLoading() override;
    const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);

	void AddWaitingOn(const std::string& waitingCondition);
	void ResetWaitingOn();

protected:
	void DrawWindowContents() override;

private:
	bool DrawSpinner(const char* label, float radius, int thickness, const ImU32& color);
	std::vector<std::string> waitingOn;
};

