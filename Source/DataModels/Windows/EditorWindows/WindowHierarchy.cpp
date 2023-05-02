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
    assert(App->scene->GetLoadedScene()->GetRoot());

    DrawRecursiveHierarchy(App->scene->GetLoadedScene()->GetRoot());

    if (App->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::REPEAT
        || App->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::DOWN)
    {
        if (App->input->GetKey(SDL_SCANCODE_C) == KeyState::DOWN)
        {
			CopyAnObject();
        }
        if (App->input->GetKey(SDL_SCANCODE_V) == KeyState::DOWN)
        {
            PasteAnObject();
        }
        if (App->input->GetKey(SDL_SCANCODE_X) == KeyState::DOWN)
        {
            CutAnObject();
        }
        if (App->input->GetKey(SDL_SCANCODE_D) == KeyState::DOWN)
        {
            DuplicateAnObject();
        }
    }

	// Delete a GameObject with the SUPR key
	if (App->input->GetKey(SDL_SCANCODE_DELETE) == KeyState::DOWN)
	{
		DeleteGameObject(App->scene->GetSelectedGameObject());
	}
}

void WindowHierarchy::DrawRecursiveHierarchy(GameObject* gameObject)
{
    assert(gameObject);

    char gameObjectLabel[160];  // Label created so ImGui can differentiate the GameObjects
                                // that have the same name in the hierarchy window
    sprintf_s(gameObjectLabel, "%s###%p", gameObject->GetName().c_str(), gameObject);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    std::vector<GameObject*> children = gameObject->GetChildren();

    if (gameObject == App->scene->GetLoadedScene()->GetRoot())
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
            && StateOfSelection::SELECTED == App->scene->GetSelectedGameObject()->GetStateOfSelection())
        {            
            ImGui::SetNextItemOpen(true);
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
    if ((ImGui::IsMouseReleased(ImGuiMouseButton_Left)
        && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
        ||
       (ImGui::IsMouseReleased(ImGuiMouseButton_Right) 
        && ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup)))
    {
        App->scene->ChangeSelectedGameObject(gameObject);
    }

    if (ImGui::BeginPopupContextItem("RightClickGameObject", ImGuiPopupFlags_MouseButtonRight))
    {
        if(gameObject->GetComponent(ComponentType::TRANSFORM) != nullptr)
        {
            if (ImGui::MenuItem("Create Empty child"))
            {
                App->scene->GetLoadedScene()->CreateGameObject("Empty GameObject", gameObject);
            }

            if (ImGui::MenuItem("Create camera"))
            {
                GameObject* newCamera =
                    App->scene->GetLoadedScene()->CreateCameraGameObject("Basic Camera", gameObject);
            }

            if (ImGui::MenuItem("Create canvas"))
            {
                GameObject* newCamera =
                    App->scene->GetLoadedScene()->CreateCanvasGameObject("Canvas", gameObject);
            }
            Create3DObjectMenu(gameObject);

            //Create Light ShortCut
            if (ImGui::BeginMenu("Create Light"))
            {
                if (ImGui::MenuItem("Spot"))
                {
                    App->scene->GetLoadedScene()->CreateLightGameObject("Spot", gameObject, LightType::SPOT);
                }
                if (ImGui::MenuItem("Point"))
                {
                    App->scene->GetLoadedScene()->CreateLightGameObject("Point", gameObject, LightType::POINT);
                }
                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("Audio"))
            {
                if (ImGui::MenuItem("Audio Source"))
                {
                    App->scene->GetLoadedScene()->CreateAudioSourceGameObject("Audio Source", gameObject);
                }
                
                ImGui::EndMenu();
            }
        }
        else
        {
            Create2DObjectMenu(gameObject);
        }

        MoveObjectMenu(gameObject, children);

        if (IsModifiable(gameObject) && ImGui::MenuItem("Delete") && gameObject != App->player->GetPlayer())
        {
            DeleteGameObject(gameObject);
            ImGui::EndPopup();
            ImGui::PopID();
            if (nodeDrawn) // If the parent node is correctly drawn, draw its children
            {
                ImGui::TreePop();
            }
            return;
        }
        ImGui::EndPopup();
    }
    ImGui::PopID();

    // The root cannot be moved around
    if (gameObject != App->scene->GetLoadedScene()->GetRoot() && ImGui::BeginDragDropSource())
    {
        UID thisID = gameObject->GetUID();
        ImGui::SetDragDropPayload("HIERARCHY", &thisID, sizeof(UID));
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY"))
        {
            UID draggedGameObjectID = *static_cast<UID*>(payload->Data); // Double pointer to keep track correctly
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
        for (GameObject* child : children)
        {
            DrawRecursiveHierarchy(child);
        }
        ImGui::TreePop();
    }
}

void WindowHierarchy::Create2DObjectMenu(GameObject* gameObject)
{
	if (ImGui::MenuItem("Create Empty 2D child"))
	{
		App->scene->GetLoadedScene()->CreateGameObject("Empty 2D GameObject", gameObject, false);
	}

	if (ImGui::MenuItem("Create Text"))
	{
		App->scene->GetLoadedScene()->CreateGameObject("new Text", gameObject, false);
	}

	if (ImGui::MenuItem("Create Image"))
	{
		App->scene->GetLoadedScene()->CreateUIGameObject("new Image", gameObject, ComponentType::IMAGE);
	}

	if (ImGui::MenuItem("Create Button"))
	{
		App->scene->GetLoadedScene()->CreateUIGameObject("new Button", gameObject, ComponentType::BUTTON);
	}
}

void WindowHierarchy::MoveObjectMenu(GameObject* gameObject, std::vector<GameObject*>& children)
{
	if (gameObject != App->scene->GetLoadedScene()->GetRoot()) // The root can't be neither deleted nor moved up/down
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
}

void WindowHierarchy::Create3DObjectMenu(GameObject* gameObject)
{
	//Create Resource
	if (ImGui::BeginMenu("Create 3D object"))
	{
		if (ImGui::MenuItem("Cube"))
		{
			App->scene->GetLoadedScene()->Create3DGameObject("Cube", gameObject, Premade3D::CUBE);
		}
		if (ImGui::MenuItem("Plane"))
		{
			App->scene->GetLoadedScene()->Create3DGameObject("Plane", gameObject, Premade3D::PLANE);
		}
		if (ImGui::MenuItem("Cylinder"))
		{
			App->scene->GetLoadedScene()->Create3DGameObject("Cylinder", gameObject, Premade3D::CYLINDER);
		}
		if (ImGui::MenuItem("Capsule"))
		{
			App->scene->GetLoadedScene()->Create3DGameObject("Capsule", gameObject, Premade3D::CAPSULE);
		}
		if (ImGui::MenuItem("Character"))
		{
			App->scene->GetLoadedScene()->Create3DGameObject("Character", gameObject, Premade3D::CHARACTER);
		}
		ImGui::EndMenu();
	}
}

bool WindowHierarchy::IsModifiable(const GameObject* gameObject) const
{
    return gameObject != App->scene->GetLoadedScene()->GetRoot() &&
        gameObject != App->scene->GetLoadedScene()->GetAmbientLight() &&
        gameObject != App->scene->GetLoadedScene()->GetDirectionalLight();
}

void WindowHierarchy::DeleteGameObject(const GameObject* gameObject) const
{
    if (!IsModifiable(gameObject))
    {
        // this object can't be deleted
        return;
    }
	if (gameObject == App->scene->GetSelectedGameObject())
	{
		App->scene->SetSelectedGameObject(gameObject->GetParent()); // If a GameObject is destroyed, 
																	// change the focus to its parent
		App->scene->GetLoadedScene()->GetRootQuadtree()->
			RemoveGameObjectAndChildren(gameObject->GetParent());
	}
	App->scene->GetLoadedScene()->GetRootQuadtree()->RemoveGameObjectAndChildren(gameObject);
	App->scene->GetLoadedScene()->RemoveNonStaticObject(gameObject);
	App->scene->GetLoadedScene()->DestroyGameObject(gameObject);
}

void WindowHierarchy::CopyAnObject()
{
	if (IsModifiable(App->scene->GetSelectedGameObject()))
	{
		copyObject = std::make_unique<GameObject>(*App->scene->GetSelectedGameObject());
	}
}

void WindowHierarchy::PasteAnObject()
{
	if (copyObject)
	{
        GameObject* parent = nullptr;
		if (App->scene->GetSelectedGameObject())
		{
			parent = App->scene->GetSelectedGameObject();
		}
		else
		{
			parent = App->scene->GetLoadedScene()->GetRoot();
		}
        App->scene->GetLoadedScene()->DuplicateGameObject(copyObject->GetName(), copyObject.get(), parent);
	}
}

void WindowHierarchy::CutAnObject()
{
    if (IsModifiable(App->scene->GetSelectedGameObject()))
	{
		CopyAnObject();

		const GameObject* gameObject = App->scene->GetSelectedGameObject();
		App->scene->SetSelectedGameObject(gameObject->GetParent()); // If a GameObject is destroyed, 
																				// change the focus to its parent
		App->scene->GetLoadedScene()->GetRootQuadtree()->
			RemoveGameObjectAndChildren(gameObject->GetParent());

		App->scene->GetLoadedScene()->RemoveNonStaticObject(gameObject);
		for (const GameObject* child : gameObject->GetChildren())
		{
			App->scene->GetLoadedScene()->RemoveNonStaticObject(child);
		}

		App->scene->GetLoadedScene()->DestroyGameObject(gameObject);
	}
}

void WindowHierarchy::DuplicateAnObject()
{
    GameObject* selectedGameObject = App->scene->GetSelectedGameObject();
	if (selectedGameObject && IsModifiable(selectedGameObject))
	{
		App->scene->GetLoadedScene()->
			DuplicateGameObject(selectedGameObject->GetName(), selectedGameObject, selectedGameObject->GetParent());
	}
}
