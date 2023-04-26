#include "WindowHierarchy.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModulePlayer.h"
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
    assert(App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot());
    DrawRecursiveHierarchy(App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot());
}

void WindowHierarchy::DrawRecursiveHierarchy(GameObject* gameObject)
{
    assert(gameObject);

    // Delete a GameObject with the SUPR key
    if (gameObject != App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot() &&
        gameObject != App->GetModule<ModuleScene>()->GetLoadedScene()->GetAmbientLight() &&
        gameObject != App->GetModule<ModuleScene>()->GetLoadedScene()->GetDirectionalLight())
    {
        if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_DELETE) == KeyState::DOWN)
        {
            if (gameObject == App->GetModule<ModuleScene>()->GetSelectedGameObject() && gameObject !=App->GetModule<ModulePlayer>()->GetPlayer())
            {
                App->GetModule<ModuleScene>()->SetSelectedGameObject(gameObject->GetParent()); // If a GameObject is destroyed, 
                                                                            // change the focus to its parent
                App->GetModule<ModuleScene>()->GetLoadedScene()->GetRootQuadtree()->
                    RemoveGameObjectAndChildren(gameObject->GetParent());

                App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(gameObject);

                return;
            }
        }
    }

    char gameObjectLabel[160];  // Label created so ImGui can differentiate the GameObjects
                                // that have the same name in the hierarchy window
    sprintf_s(gameObjectLabel, "%s###%p", gameObject->GetName().c_str(), gameObject);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    std::vector<GameObject*> children = gameObject->GetChildren();

    if (gameObject == App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot())
    {
        flags |= ImGuiTreeNodeFlags_DefaultOpen;
    }
    else
    {
        if (children.empty())
        {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }

        if (gameObject->GetStateOfSelection() == StateOfSelection::CHILD_SELECTED
            && StateOfSelection::SELECTED == App->GetModule<ModuleScene>()->GetSelectedGameObject()->GetStateOfSelection())
        {            
            ImGui::SetNextItemOpen(true);
        }
    }

    if (gameObject == App->GetModule<ModuleScene>()->GetSelectedGameObject())
    {
        flags |= ImGuiTreeNodeFlags_Selected;
    }

    ImGui::PushStyleColor(0, (gameObject->IsEnabled() && gameObject->IsActive()) ? white : grey);
    bool nodeDrawn = ImGui::TreeNodeEx(gameObjectLabel, flags);
    ImGui::PopStyleColor();

    ImGui::PushID(gameObjectLabel);
    if ((ImGui::IsMouseReleased(ImGuiMouseButton_Left)
        && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
        ||
       (ImGui::IsMouseReleased(ImGuiMouseButton_Right) 
        && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup)))
    {
        App->GetModule<ModuleScene>()->ChangeSelectedGameObject(gameObject);
    }

    if (ImGui::BeginPopupContextItem("RightClickGameObject", ImGuiPopupFlags_MouseButtonRight))
    {
        if(gameObject->GetComponent(ComponentType::TRANSFORM) != nullptr)
        {
            if (ImGui::MenuItem("Create Empty child"))
            {
                App->GetModule<ModuleScene>()->GetLoadedScene()->CreateGameObject("Empty GameObject", gameObject);
            }

            if (ImGui::MenuItem("Create camera"))
            {
                GameObject* newCamera =
                    App->GetModule<ModuleScene>()->GetLoadedScene()->CreateCameraGameObject("Basic Camera", gameObject);
            }

            if (ImGui::MenuItem("Create canvas"))
            {
                GameObject* newCamera =
                    App->GetModule<ModuleScene>()->GetLoadedScene()->CreateCanvasGameObject("Canvas", gameObject);
            }
            //Create Resource
            if (ImGui::BeginMenu("Create 3D object"))
            {
                if (ImGui::MenuItem("Cube"))
                {
                    App->GetModule<ModuleScene>()->GetLoadedScene()->Create3DGameObject("Cube", gameObject, Premade3D::CUBE);
                }
                if (ImGui::MenuItem("Plane"))
                {
                    App->GetModule<ModuleScene>()->GetLoadedScene()->Create3DGameObject("Plane", gameObject, Premade3D::PLANE);
                }
                if (ImGui::MenuItem("Cylinder"))
                {
                    App->GetModule<ModuleScene>()->GetLoadedScene()->Create3DGameObject("Cylinder", gameObject, Premade3D::CYLINDER);
                }
                if (ImGui::MenuItem("Capsule"))
                {
                    App->GetModule<ModuleScene>()->GetLoadedScene()->Create3DGameObject("Capsule", gameObject, Premade3D::CAPSULE);
                }
                if (ImGui::MenuItem("Character"))
                {
                    App->GetModule<ModuleScene>()->GetLoadedScene()->Create3DGameObject("Character", gameObject, Premade3D::CHARACTER);
                }
                ImGui::EndMenu();
            }
            //Create Light ShortCut
            if (ImGui::BeginMenu("Create Light"))
            {
                if (ImGui::MenuItem("Spot"))
                {
                    App->GetModule<ModuleScene>()->GetLoadedScene()->CreateLightGameObject("Spot", gameObject, LightType::SPOT);
                }
                if (ImGui::MenuItem("Point"))
                {
                    App->GetModule<ModuleScene>()->GetLoadedScene()->CreateLightGameObject("Point", gameObject, LightType::POINT);
                }
                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("Audio"))
            {
                if (ImGui::MenuItem("Audio Source"))
                {
                    App->GetModule<ModuleScene>()->GetLoadedScene()->CreateAudioSourceGameObject("Audio Source", gameObject);
                }
                
                ImGui::EndMenu();
            }
        }
        else
        {
            if (ImGui::MenuItem("Create Empty 2D child"))
            {
                App->GetModule<ModuleScene>()->GetLoadedScene()->CreateGameObject("Empty 2D GameObject", gameObject, false);
            }

            if (ImGui::MenuItem("Create Text"))
            {
                App->GetModule<ModuleScene>()->GetLoadedScene()->CreateGameObject("new Text", gameObject, false);
            }

            if (ImGui::MenuItem("Create Image"))
            {
                App->GetModule<ModuleScene>()->GetLoadedScene()->CreateUIGameObject("new Image", gameObject, ComponentType::IMAGE);
            }

            if (ImGui::MenuItem("Create Button"))
            {
                App->GetModule<ModuleScene>()->GetLoadedScene()->CreateUIGameObject("new Button", gameObject, ComponentType::BUTTON);
            }
        }

        if (gameObject != App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot()) // The root can't be neither deleted nor moved up/down
        {
            if (ImGui::MenuItem("Move Up"))
            {
                if (!children.empty()
                    && children[0] != gameObject)
                {
                    gameObject->GetParent()->MoveUpChild(gameObject);
                }
            }

            if (ImGui::MenuItem("Move Down"))
            {
                if (!children.empty()
                    && children[children.size() - 1] != gameObject)
                {
                    gameObject->GetParent()->MoveDownChild(gameObject);
                }
            }
        }

        if (gameObject != App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot() &&
            gameObject != App->GetModule<ModuleScene>()->GetLoadedScene()->GetAmbientLight() &&
            gameObject != App->GetModule<ModuleScene>()->GetLoadedScene()->GetDirectionalLight())
        {
            if (ImGui::MenuItem("Delete") && gameObject != App->GetModule<ModulePlayer>()->GetPlayer())
            {
                if (gameObject == App->GetModule<ModuleScene>()->GetSelectedGameObject())
                {
                    App->GetModule<ModuleScene>()->SetSelectedGameObject(gameObject->GetParent()); // If a GameObject is destroyed, 
                                                                                // change the focus to its parent
                    App->GetModule<ModuleScene>()->RemoveGameObjectAndChildren(gameObject->GetParent());
                }
                App->GetModule<ModuleScene>()->RemoveGameObjectAndChildren(gameObject);
                App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(gameObject);

                ImGui::EndPopup();
                ImGui::PopID();
                if (nodeDrawn) // If the parent node is correctly drawn, draw its children
                {
                    ImGui::TreePop();
                }
                return;
            }
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();

    if (gameObject != App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot()) // The root cannot be moved around
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
                App->GetModule<ModuleScene>()->GetLoadedScene()->SearchGameObjectByID(draggedGameObjectID);
            if (draggedGameObject)
            {
                draggedGameObject->MoveParent(gameObject);
            }
        }

        ImGui::EndDragDropTarget();
    }

    if (nodeDrawn) // If the parent node is correctly drawn, draw its children
    {
        for (GameObject* child : children)
        {
            DrawRecursiveHierarchy(child);
        }
        ImGui::TreePop();
    }
}
