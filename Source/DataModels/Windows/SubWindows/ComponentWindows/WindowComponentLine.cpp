#include "WindowComponentLine.h"
#include "Components/ComponentLine.h"

WindowComponentLine::WindowComponentLine(ComponentLine* component) :
	ComponentWindow("Line", component)
{
}

WindowComponentLine::~WindowComponentLine()
{
}

void WindowComponentLine::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ImGui::Text(""); // used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent

	ComponentLine* componentLine = static_cast<ComponentLine*>(component);

	if (componentLine)
	{

		float numTiles = componentLine->GetNumTiles();
		ImGui::Text("");
		ImGui::Text("Number of tiles");
		ImGui::SameLine();
		if (ImGui::DragFloat("Tiles", &numTiles, 1.0f, 1.0f, 50.0f))
		{
			
			componentLine->SetNumTiles(numTiles);
			
		}
	}
}