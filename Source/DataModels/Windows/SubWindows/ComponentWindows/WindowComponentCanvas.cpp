#include "WindowComponentCanvas.h"

#include "Components/ComponentCanvas.h"


WindowComponentCanvas::WindowComponentCanvas(ComponentCanvas* component) :
	ComponentWindow("CANVAS", component)
{
}

WindowComponentCanvas::~WindowComponentCanvas()
{
}

void WindowComponentCanvas::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text("");
}
