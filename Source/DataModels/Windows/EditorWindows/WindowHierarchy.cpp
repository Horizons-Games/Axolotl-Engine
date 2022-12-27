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
    //int gameObjectSelected = -1;
    for (int i = 0; i < 5; i++)
    {
        if (ImGui::TreeNode((void*)(intptr_t)i, "GameObject %d", i))
        {
            if (ImGui::IsItemClicked(1))
            {
                ImGui::OpenPopup("RightClickGameObject");
                //gameObjectSelected = i;
            }
            if (ImGui::BeginPopup("RightClickGameObject"))
            {
                ImGui::MenuItem("Create child");
                ImGui::MenuItem("Delete");

                ImGui::EndPopup();
            }

            ImGui::Text("blah blah");

            ImGui::TreePop();
        }

        if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
        {
            ImGui::OpenPopup("RightClickHierarchy", ImGuiPopupFlags_NoOpenOverExistingPopup);
        }
        if (ImGui::IsItemClicked(1))
        {
            ImGui::OpenPopup("RightClickGameObject");
            //gameObjectSelected = i;
        }
    }

    if (ImGui::BeginPopup("RightClickGameObject"))
    {
        ImGui::MenuItem("Create child");
        ImGui::MenuItem("Delete");

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("RightClickHierarchy"))
    {
        ImGui::MenuItem("Create empty GameObject");

        ImGui::EndPopup();
    }
}



