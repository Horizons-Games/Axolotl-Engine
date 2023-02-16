#pragma once

#include "EditorWindow.h"

class WindowLoading : public EditorWindow
{
public:
	WindowLoading();
	~WindowLoading();
    const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;

private:
	bool DrawSpinner(const char* label, float radius, int thickness, const ImU32& color);
};

inline ImVec2 WindowLoading::GetStartingSize() const
{
	return ImVec2(250, 150);
}
