#include "WindowHierarchy.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "ModuleInput.h"
#include "GameObject/GameObject.h"

static ImVec4 grey = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
static ImVec4 white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

WindowHierarchy::WindowHierarchy() : EditorWindow("Hierarchy"), stopDrawing(false)
{
    flags |= ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar;
}

WindowHierarchy::~WindowHierarchy()
{
}

void WindowHierarchy::DrawWindowContents()
{
    if (App->scene->GetLoadedScene()->GetRoot())
    {
        DrawRecursiveHierarchy(App->scene->GetLoadedScene()->GetRoot(), true);
    }
}

void WindowHierarchy::DrawRecursiveHierarchy(GameObject* gameObject, bool isRoot)
{
    if (isRoot)
    {
        stopDrawing = false;
    }
    else if (stopDrawing)
    {
        return;
    }

    assert(gameObject != nullptr);

    char gameObjectLabel[160];  // Label created so ImGui can differentiate the GameObjects
                                // that have the same name in the hierarchy window
    sprintf_s(gameObjectLabel, "%s###%p", gameObject->GetName(), gameObject);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (gameObject->GetChildren().empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf;
    }

    if (isRoot)
    {
        flags |= ImGuiTreeNodeFlags_DefaultOpen;
    }
    else
    {
        if (gameObject->IsChildSelected())
        {
            ImGui::SetNextItemOpen(true, ImGuiCond_Always);
        }

        /*const std::list<GameObject*>& childrenList = gameObject->GetGameObjectsInside();
        for (GameObject* child : childrenList)
        {
            if (child->GetStateOfSelection() == StateOfSelection::SELECTED)
            {
                ImGui::SetNextItemOpen(true);
            }
        }*/
    }

    if (gameObject == App->scene->GetSelectedGameObject())
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    ImGui::PushStyleColor(0, (gameObject->IsEnabled() && gameObject->IsActive()) ? white : grey);
    bool nodeDrawn = ImGui::TreeNodeEx(gameObjectLabel, flags);
    ImGui::PopStyleColor();

    bool itemVisible = ImGui::IsItemVisible();
    bool itemAboveCurrentScroll = ImGui::GetScrollY() >=
        ImGui::GetItemRectMin().y - ImGui::GetWindowContentRegionMin().y - ImGui::GetWindowPos().y;

    if (nodeDrawn)
    {
        if (itemVisible)
        {
            ImGui::PushID(gameObjectLabel);
            if (ImGui::IsItemClicked(ImGuiMouseButton_Left) ||
                (ImGui::IsMouseClicked(ImGuiMouseButton_Right)
                    && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup)))
            {
                App->scene->GetLoadedScene()->GetSceneQuadTree()
                    ->AddGameObjectAndChildren(App->scene->GetSelectedGameObject());
                gameObject->SetStateOfSelection(StateOfSelection::NO_SELECTED);
                App->scene->SetSelectedGameObject(gameObject);
                App->scene->GetLoadedScene()->GetSceneQuadTree()->RemoveGameObjectAndChildren(gameObject);
            }

            DrawPopupMenu(gameObject);
            ImGui::PopID();

            if (!isRoot) // The root cannot be moved around
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
        }
        if (itemVisible || itemAboveCurrentScroll)
        {
            for (int i = 0; i < gameObject->GetChildren().size(); ++i)
            {
                DrawRecursiveHierarchy(gameObject->GetChildren()[i], false);
            }
        }
        ImGui::TreePop();
    }

    if (!(itemVisible || itemAboveCurrentScroll))
    {
        stopDrawing = true;
    }
}

void WindowHierarchy::DrawPopupMenu(GameObject* gameObject)
{
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
}
