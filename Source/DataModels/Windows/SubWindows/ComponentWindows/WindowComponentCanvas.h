#pragma once

#include "ComponentWindow.h"

class ComponentCanvas;

class WindowComponentCanvas : public ComponentWindow
{
public:
	WindowComponentCanvas(ComponentCanvas* component);
	~WindowComponentCanvas() override;

protected:
	void DrawWindowContents() override;
};

inline void WindowComponentCanvas::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text("");
}
