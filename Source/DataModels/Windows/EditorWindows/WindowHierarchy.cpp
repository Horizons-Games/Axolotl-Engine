#include "WindowHierarchy.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "ModuleInput.h"
#include "GameObject/GameObject.h"

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
    if (App->scene->GetLoadedScene()->GetRoot())
    {
        DrawRecursiveHierarchy(App->scene->GetLoadedScene()->GetRoot());
    }
}

void WindowHierarchy::DrawRecursiveHierarchy(GameObject* gameObject)
{
    assert(gameObject != nullptr);

    char gameObjectLabel[160];  // Label created so ImGui can differentiate the GameObjects
                                // that have the same name in the hierarchy window
    sprintf_s(gameObjectLabel, "%s###%p", gameObject->GetName(), gameObject);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (gameObject->GetChildren().empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (gameObject == App->scene->GetLoadedScene()->GetRoot())
    {
        flags |= ImGuiTreeNodeFlags_DefaultOpen;
    }
    else
    {
        const std::list<GameObject*>& childrenList = gameObject->GetGameObjectsInside();
        for (GameObject* child : childrenList)
        {
            if (child == App->scene->GetSelectedGameObject()
                && StateOfSelection::SELECTED == App->scene->GetSelectedGameObject()->GetStateOfSelection())
            {
                ImGui::SetNextItemOpen(true);
            }
        }
    }

    if (gameObject == App->scene->GetSelectedGameObject())
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    ImGui::PushStyleColor(0, (gameObject->IsEnabled() && gameObject->IsActive()) ? white : grey);
    bool nodeDrawn = ImGui::TreeNodeEx(gameObjectLabel, flags);
    ImGui::PopStyleColor();

    ImGui::PushID(gameObjectLabel);
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left) ||
        (ImGui::IsMouseClicked(ImGuiMouseButton_Right)
            && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup)))
    {
        App->scene->GetLoadedScene()->GetSceneQuadTree()
            ->AddGameObjectAndChildren(App->scene->GetSelectedGameObject());
        App->scene->SetSelectedGameObject(gameObject);
        App->scene->GetLoadedScene()->GetSceneQuadTree()->RemoveGameObjectAndChildren(gameObject);
    }

    if (ImGui::BeginPopupContextItem("RightClickGameObject", ImGuiPopupFlags_MouseButtonRight))
    {

        if (ImGui::MenuItem("Create child"))
        {
            App->scene->GetLoadedScene()->CreateGameObject("Empty GameObject", gameObject);
        }
        if (ImGui::MenuItem("Create camera"))
        {
            GameObject* newCamera =
                App->scene->GetLoadedScene()->CreateCameraGameObject("Basic Camera", gameObject);
        }

        if (gameObject != App->scene->GetLoadedScene()->GetRoot()) // The root can't be neither deleted nor moved up/down
        {
            GameObject* selectedParent = gameObject->GetParent();
            std::vector<GameObject*> parentsChildren = selectedParent->GetChildren();

            if (ImGui::MenuItem("Move Up"))
            {
                if (parentsChildren.size() > 1 && parentsChildren[0] != gameObject)
                {
                    selectedParent->MoveUpChild(gameObject);
                }
            }

            if (ImGui::MenuItem("Move Down"))
            {
                if (parentsChildren.size() > 1 && parentsChildren[parentsChildren.size() - 1] != gameObject)
                {
                    selectedParent->MoveDownChild(gameObject);
                }
            }
        }

        if (gameObject != App->scene->GetLoadedScene()->GetRoot() &&
            gameObject != App->scene->GetLoadedScene()->GetAmbientLight() &&
            gameObject != App->scene->GetLoadedScene()->GetDirectionalLight())
        {
            if (ImGui::MenuItem("Delete"))
            {
                if (gameObject == App->scene->GetSelectedGameObject())
                {
                    App->scene->SetSelectedGameObject(gameObject->GetParent()); // If a GameObject is destroyed, 
                                                                                // change the focus to its parent
                    App->scene->GetLoadedScene()->GetSceneQuadTree()->
                        RemoveGameObjectAndChildren(gameObject->GetParent());
                }
                App->scene->GetLoadedScene()->GetSceneQuadTree()->RemoveGameObjectAndChildren(gameObject);
                App->scene->GetLoadedScene()->DestroyGameObject(gameObject);
            }
        }

        ImGui::EndPopup();
    }
    ImGui::PopID();

    if (gameObject != App->scene->GetLoadedScene()->GetRoot()) // The root cannot be moved around
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
            GameObject* draggedGameObject =
                App->scene->GetLoadedScene()->SearchGameObjectByID(draggedGameObjectID);
            if (draggedGameObject)
            {
                draggedGameObject->SetParent(gameObject);
            }
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