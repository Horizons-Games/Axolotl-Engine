#include "WindowHierarchy.h"

#include "imgui.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "GameObject/GameObject.h"

#include "3DModels/Model.h"

#include <string>

static ImVec4 grey = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
static ImVec4 white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

WindowHierarchy::WindowHierarchy() : EditorWindow("Hierarchy")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;
}

WindowHierarchy::~WindowHierarchy()
{
}

void WindowHierarchy::DrawWindowContents()
{
    if (App->scene->GetRoot() != nullptr)
    {
        DrawRecursiveHierarchy(App->scene->GetRoot());
    }

    // Create Empty GameObjects in the root
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsWindowHovered())
    {
        ImGui::OpenPopup("RightClickHierarchy", ImGuiPopupFlags_NoOpenOverExistingPopup);
    }

    if (ImGui::BeginPopup("RightClickHierarchy"))
    {
        if (ImGui::MenuItem("Create empty GameObject"))
        {
            App->scene->GetRoot()->AddChild(new GameObject("Empty", App->scene->GetRoot()));
        }

        ImGui::EndPopup();
    }
}

void WindowHierarchy::DrawRecursiveHierarchy(GameObject* gameObject)
{
    char gameObjectId[160]; // ID created so ImGui can differentiate the GameObjects
                            // that have the same name in the hierarchy window
    sprintf_s(gameObjectId, "%s###%p", gameObject->GetName(), gameObject);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
    if (gameObject->GetChildren().empty()) flags |= ImGuiTreeNodeFlags_Leaf;
    if (gameObject == App->scene->GetRoot()) flags |= ImGuiTreeNodeFlags_DefaultOpen;

    ImGui::PushStyleColor(0, gameObject->GetActive() ? white : grey);

    bool nodeDrawn = ImGui::TreeNodeEx(gameObjectId, flags);

    ImGui::PushID(gameObjectId);
    if (ImGui::BeginPopupContextItem("RightClickGameObject", ImGuiPopupFlags_MouseButtonRight))
    {
        if (ImGui::MenuItem("Rename"))
        {
            ENGINE_LOG("aaaaaaaaa");
        }

        if (ImGui::MenuItem("Delete"))
        {
            ENGINE_LOG("bbbbbbbbb");
        }

        if (ImGui::MenuItem("Create child"))
        {
            ENGINE_LOG("ccccccccc");
        }

        ImGui::EndPopup();
    }
    ImGui::PopID();

    if (nodeDrawn) // If the parent node is correctly drawn, draw its children
    {
        for (int i = 0; i < gameObject->GetChildren().size(); ++i)
        {
            DrawRecursiveHierarchy(gameObject->GetChildren()[i]);
        }

        ImGui::TreePop();
    }

    ImGui::PopStyleColor();
}