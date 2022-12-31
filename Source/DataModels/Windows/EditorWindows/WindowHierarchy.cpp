#include "WindowHierarchy.h"

#include "imgui.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "GameObject/GameObject.h"

#include "3DModels/Model.h"

#include <string>

WindowHierarchy::WindowHierarchy() : EditorWindow("Hierarchy")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;
}

WindowHierarchy::~WindowHierarchy()
{
}

void WindowHierarchy::DrawWindowContents()
{
    for (int i = 0; i < App->scene->GetRoot()->GetChildren().size(); ++i)
    {
        if (ImGui::TreeNodeEx(App->scene->GetRoot()->GetChildren()[i]->GetName(), ImGuiTreeNodeFlags_Leaf))
        {
            if (ImGui::BeginPopup("RightClickGameObject"))
            {
                ImGui::MenuItem("Create child");
                ImGui::MenuItem("Delete");

                ImGui::EndPopup();
            }

            ImGui::TreePop();
        }

        if (ImGui::IsItemClicked(1))
        {
            ImGui::OpenPopup("RightClickGameObject");
        }
    }

    if (ImGui::BeginPopup("RightClickGameObject"))
    {
        if (ImGui::MenuItem("Create child"))
        {
            ENGINE_LOG("aaaaaaaaaaa");
        }

        if (ImGui::MenuItem("Delete"))
        {
            ENGINE_LOG("bbbbbbbbbbb");
        }

        ImGui::EndPopup();
    }

    // Create Empty GameObjects
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
    {
        ImGui::OpenPopup("RightClickHierarchy", ImGuiPopupFlags_NoOpenOverExistingPopup);
    }

    if (ImGui::BeginPopup("RightClickHierarchy"))
    {
        if (ImGui::MenuItem("Create empty GameObject"))
        {
            std::string newName = "Empty (0)";

            for (int i = 0; i < App->scene->GetRoot()->GetChildren().size(); ++i)
            {
                newName = "Empty (" + std::to_string(i + 1) + ")";
            }

            App->scene->GetRoot()->AddChild(new GameObject(newName.c_str(), App->scene->GetRoot()));
        }

        ImGui::EndPopup();
    }
}



