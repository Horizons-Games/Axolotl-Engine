#include "WindowHierarchy.h"

#include "imgui.h"

#include "Application.h"
#include "ModuleRender.h"

#include "3DModels/Model.h"

WindowHierarchy::WindowHierarchy() : EditorWindow("Hierarchy")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;
}

WindowHierarchy::~WindowHierarchy()
{
}

void WindowHierarchy::DrawWindowContents()
{
	

    if (ImGui::TreeNode("Scene:GameObjects"))
    {
        for (int i = 0; i < 5; i++)
        {
            
            if (i == 0)
                ImGui::SetNextItemOpen(true, ImGuiCond_Once);

            if (ImGui::TreeNode((void*)(intptr_t)i, "GameObject %d", i))
            {
                ImGui::Text("blah blah");
                //ImGui::SameLine();
                //if (ImGui::SmallButton("button")) {}
                ImGui::TreePop();
            }
        }
        ImGui::TreePop();
    }
	
}



