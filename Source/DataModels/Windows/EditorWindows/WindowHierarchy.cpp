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
    GameObject* root = App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot();
    assert(root);
    DrawRecursiveHierarchy(root);

    ModuleInput* input = App->GetModule<ModuleInput>();

    if (input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::REPEAT
        || input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::DOWN)
    {
        if (input->GetKey(SDL_SCANCODE_C) == KeyState::DOWN)
        {
			CopyAnObject();
        }
        if (input->GetKey(SDL_SCANCODE_V) == KeyState::DOWN)
        {
            PasteAnObject();
        }
        if (input->GetKey(SDL_SCANCODE_X) == KeyState::DOWN)
        {
            CutAnObject();
        }
        if (input->GetKey(SDL_SCANCODE_D) == KeyState::DOWN)
        {
            DuplicateAnObject();
        }
    }

	// Delete a GameObject with the SUPR key
	if (input->GetKey(SDL_SCANCODE_DELETE) == KeyState::DOWN)
	{
		DeleteGameObject(App->GetModule<ModuleScene>()->GetSelectedGameObject());
	}
}

void WindowHierarchy::DrawRecursiveHierarchy(GameObject* gameObject)
{
    assert(gameObject);

    ModuleScene* moduleScene = App->GetModule<ModuleScene>();
    Scene* loadedScene = moduleScene->GetLoadedScene();
    
    char gameObjectLabel[160];  // Label created so ImGui can differentiate the GameObjects
                                // that have the same name in the hierarchy window
    sprintf_s(gameObjectLabel, "%s###%p", gameObject->GetName().c_str(), gameObject);

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    std::vector<GameObject*> children = gameObject->GetChildren();

    if (gameObject == loadedScene->GetRoot())
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
            && StateOfSelection::SELECTED == moduleScene->GetSelectedGameObject()->GetStateOfSelection())
        {            
            ImGui::SetNextItemOpen(true);
        }
    }

    if (gameObject == moduleScene->GetSelectedGameObject())
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
        moduleScene->ChangeSelectedGameObject(gameObject);
    }

    if (ImGui::BeginPopupContextItem("RightClickGameObject", ImGuiPopupFlags_MouseButtonRight))
    {
        if(gameObject->GetComponent(ComponentType::TRANSFORM) != nullptr)
        {
            if (ImGui::MenuItem("Create Empty child"))
            {
                loadedScene->CreateGameObject("Empty GameObject", gameObject);
            }

            if (ImGui::MenuItem("Create camera"))
            {
                GameObject* newCamera =
                    loadedScene->CreateCameraGameObject("Basic Camera", gameObject);
            }

            if (ImGui::MenuItem("Create canvas"))
            {
                GameObject* newCamera =
                    loadedScene->CreateCanvasGameObject("Canvas", gameObject);
            }
            Create3DObjectMenu(gameObject);

            //Create Light ShortCut
            if (ImGui::BeginMenu("Create Light"))
            {
                if (ImGui::MenuItem("Spot"))
                {
                    loadedScene->CreateLightGameObject("Spot", gameObject, LightType::SPOT);
                }
                if (ImGui::MenuItem("Point"))
                {
                    loadedScene->CreateLightGameObject("Point", gameObject, LightType::POINT);
                }
                ImGui::EndMenu();
            }
            
            if (ImGui::BeginMenu("Audio"))
            {
                if (ImGui::MenuItem("Audio Source"))
                {
                    loadedScene->CreateAudioSourceGameObject("Audio Source", gameObject);
                }
                
                ImGui::EndMenu();
            }
        }
        else
        {
            Create2DObjectMenu(gameObject);
        }

        MoveObjectMenu(gameObject, children);

        if (IsModifiable(gameObject) && ImGui::MenuItem("Delete") && gameObject != App->GetModule<ModulePlayer>()->GetPlayer())
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
    if (gameObject != loadedScene->GetRoot() && ImGui::BeginDragDropSource())
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
                loadedScene->SearchGameObjectByID(draggedGameObjectID);
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
    Scene* loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	if (ImGui::MenuItem("Create Empty 2D child"))
	{
		loadedScene->CreateGameObject("Empty 2D GameObject", gameObject, false);
	}

	if (ImGui::MenuItem("Create Text"))
	{
		loadedScene->CreateGameObject("new Text", gameObject, false);
	}

	if (ImGui::MenuItem("Create Image"))
	{
		loadedScene->CreateUIGameObject("new Image", gameObject, ComponentType::IMAGE);
	}

	if (ImGui::MenuItem("Create Button"))
	{
		loadedScene->CreateUIGameObject("new Button", gameObject, ComponentType::BUTTON);
	}
}

void WindowHierarchy::MoveObjectMenu(GameObject* gameObject, std::vector<GameObject*>& children)
{
    // The root can't be neither deleted nor moved up/down
	if (gameObject != App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot())
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
    Scene* loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	//Create Resource
	if (ImGui::BeginMenu("Create 3D object"))
	{
		if (ImGui::MenuItem("Cube"))
		{
            loadedScene->Create3DGameObject("Cube", gameObject, Premade3D::CUBE);
		}
		if (ImGui::MenuItem("Plane"))
		{
            loadedScene->Create3DGameObject("Plane", gameObject, Premade3D::PLANE);
		}
		if (ImGui::MenuItem("Cylinder"))
		{
            loadedScene->Create3DGameObject("Cylinder", gameObject, Premade3D::CYLINDER);
		}
		if (ImGui::MenuItem("Capsule"))
		{
            loadedScene->Create3DGameObject("Capsule", gameObject, Premade3D::CAPSULE);
		}
		if (ImGui::MenuItem("Character"))
		{
            loadedScene->Create3DGameObject("Character", gameObject, Premade3D::CHARACTER);
		}
		ImGui::EndMenu();
	}
}

bool WindowHierarchy::IsModifiable(const GameObject* gameObject) const
{
    Scene* loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

    return gameObject != loadedScene->GetRoot() &&
        gameObject != loadedScene->GetAmbientLight() &&
        gameObject != loadedScene->GetDirectionalLight();
}

void WindowHierarchy::DeleteGameObject(const GameObject* gameObject) const
{
    if (!IsModifiable(gameObject))
    {
        // this object can't be deleted
        return;
    }

    ModuleScene* moduleScene = App->GetModule<ModuleScene>();
    Scene* loadedScene = moduleScene->GetLoadedScene();

	if (gameObject == moduleScene->GetSelectedGameObject())
	{
		moduleScene->SetSelectedGameObject(gameObject->GetParent()); // If a GameObject is destroyed, 
																	// change the focus to its parent
		loadedScene->GetRootQuadtree()->
			RemoveGameObjectAndChildren(gameObject->GetParent());
	}
	loadedScene->GetRootQuadtree()->RemoveGameObjectAndChildren(gameObject);
	loadedScene->RemoveNonStaticObject(gameObject);
	loadedScene->DestroyGameObject(gameObject);
}

void WindowHierarchy::CopyAnObject()
{
    GameObject* selectedGameObject = App->GetModule<ModuleScene>()->GetSelectedGameObject();

	if (IsModifiable(selectedGameObject))
	{
		copyObject = std::make_unique<GameObject>(*selectedGameObject);
	}
}

void WindowHierarchy::PasteAnObject()
{
	if (copyObject)
	{
		ModuleScene* moduleScene = App->GetModule<ModuleScene>();
		Scene* loadedScene = moduleScene->GetLoadedScene();
        GameObject* selectedGameObject = moduleScene->GetSelectedGameObject();

        GameObject* parent = selectedGameObject;
		if (parent == nullptr)
		{
			parent = loadedScene->GetRoot();
		}
        loadedScene->DuplicateGameObject(copyObject->GetName(), copyObject.get(), parent);
	}
}

void WindowHierarchy::CutAnObject()
{
	ModuleScene* moduleScene = App->GetModule<ModuleScene>();
	Scene* loadedScene = moduleScene->GetLoadedScene();
	GameObject* selectedGameObject = moduleScene->GetSelectedGameObject();

    if (IsModifiable(selectedGameObject))
	{
		CopyAnObject();

		moduleScene->SetSelectedGameObject(selectedGameObject->GetParent()); // If a GameObject is destroyed, 
																			 // change the focus to its parent
		loadedScene->GetRootQuadtree()->RemoveGameObjectAndChildren(selectedGameObject->GetParent());

		loadedScene->RemoveNonStaticObject(selectedGameObject);
		for (const GameObject* child : selectedGameObject->GetChildren())
		{
			loadedScene->RemoveNonStaticObject(child);
		}

		loadedScene->DestroyGameObject(selectedGameObject);
	}
}

void WindowHierarchy::DuplicateAnObject()
{
	ModuleScene* moduleScene = App->GetModule<ModuleScene>();
	Scene* loadedScene = moduleScene->GetLoadedScene();
	GameObject* selectedGameObject = moduleScene->GetSelectedGameObject();

	if (selectedGameObject && IsModifiable(selectedGameObject))
	{
		loadedScene->
            DuplicateGameObject(selectedGameObject->GetName(), selectedGameObject, selectedGameObject->GetParent());
	}
}
