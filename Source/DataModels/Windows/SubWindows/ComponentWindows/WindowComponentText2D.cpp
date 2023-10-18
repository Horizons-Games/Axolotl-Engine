#include "StdAfx.h"
#include "WindowComponentText2D.h"

#include "DataModels/Components/UI/ComponentText2D.h"
#include "Resources/ResourceFont.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowFontInput.h"


WindowComponentText2D::WindowComponentText2D(ComponentText2D* component) :
	ComponentWindow("TEXT2D", component),
	componentText2D(component),
	inputFont(std::make_unique<WindowFontInput>()),
	str1("")
{
}

WindowComponentText2D::~WindowComponentText2D()
{
}

void WindowComponentText2D::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();	
		
	std::string items;
	std::string texts;
	
	for (std::shared_ptr<ResourceFont> i : componentText2D->GetFonts())
	{
		items+=i->GetFileName();
		items += '\0';
	}
	if (componentText2D)
	{			
		static int item = -1;	
		static float col1[3] = { 1.0f, 0.0f, 0.2f };				
		inputFont->DrawWindowContents();
		ImGui::Combo("Fonts", &item, items.c_str());		
		ImGui::InputTextWithHint("input text", "enter text here", str1, IM_ARRAYSIZE(str1));
		ImGui::ColorEdit3("Color",  col1);
		if (ImGui::Button("submit")) 
		{			
			if (item != -1)
			{
				componentText2D->SetCharacters((unsigned int) item);
				componentText2D->SetText(std::string(str1));
				componentText2D->SetColor(float3(col1[0], col1[1], col1[2]));
			}				
		}		
	}
}