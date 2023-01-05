#include "WindowHierarchy.h"

#include "imgui.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "GameObject/GameObject.h"

#include "3DModels/Model.h"

#include <string>
#include <assert.h>

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
}

void WindowHierarchy::DrawRecursiveHierarchy(GameObject* gameObject)
{
    assert(gameObject != nullptr);

    char gameObjectLabel[160];  // Label created so ImGui can differentiate the GameObjects
                                // that have the same name in the hierarchy window
    sprintf_s(gameObjectLabel, "%s###%p", gameObject->GetName(), gameObject);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
    if (gameObject->GetChildren().empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (gameObject == App->scene->GetRoot())
    {
        flags |= ImGuiTreeNodeFlags_DefaultOpen;
    }
    
    if (App->scene->GetSelectedGameObject() == gameObject)
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    ImGui::PushStyleColor(0, (gameObject->IsEnabled() && gameObject->IsActive()) ? white : grey);
    bool nodeDrawn = ImGui::TreeNodeEx(gameObjectLabel, flags);
    ImGui::PopStyleColor();

    if (ImGui::IsItemClicked())
    {
        App->scene->SetSelectedGameObject(gameObject);
    }

    ImGui::PushID(gameObjectLabel);
    if (ImGui::BeginPopupContextItem("RightClickGameObject", ImGuiPopupFlags_MouseButtonRight))
    {
        if (gameObject != App->scene->GetRoot()) // The root can neither be renamed nor deleted
        {
            if (ImGui::MenuItem("Rename"))
            {
                gameObject->SetName("Renamed GameObject");
            }

            if (ImGui::MenuItem("Delete"))
            {
                gameObject->GetParent()->RemoveChild(gameObject);
                if (App->scene->GetSelectedGameObject() == gameObject)
                {
                    App->scene->SetSelectedGameObject(gameObject->GetParent());
                }
                delete gameObject;
            }
        }

        if (ImGui::MenuItem("Create child"))
        {
            App->scene->CreateGameObject("Empty GameObject", gameObject);
        }

        ImGui::EndPopup();
    }
    ImGui::PopID();

    if (gameObject != App->scene->GetRoot()) // The root cannot be moved around
    {
        if (ImGui::BeginDragDropSource())
        {
            UID thisID = gameObject->GetUID();
            ImGui::SetDragDropPayload("HIERARCHY", &thisID, sizeof(UID));

            ImGui::EndDragDropSource();
        }
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY"))
        {
            UID draggedGameObjectID = *(UID*)payload->Data; // Double pointer to keep track correctly
                                                            // of the UID of the dragged GameObject
            GameObject* draggedGameObject = App->scene->SearchGameObjectByID(draggedGameObjectID);

            draggedGameObject->SetParent(gameObject);
        }

        ImGui::EndDragDropTarget();
    }

    if (nodeDrawn) // If the parent node is correctly drawn, draw its children
    {
        for (int i = 0; i < gameObject->GetChildren().size(); ++i)
        {
            DrawRecursiveHierarchy(gameObject->GetChildren()[i]);
        }

        ImGui::TreePop();
    }
}