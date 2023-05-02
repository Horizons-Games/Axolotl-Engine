#include "WindowHierarchy.h"

#include "Application.h"
#include "GameObject/GameObject.h"
#include "ModuleInput.h"
#include "ModulePlayer.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

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
}

void WindowHierarchy::DrawRecursiveHierarchy(GameObject* gameObject)
{
	assert(gameObject);

	ModuleScene* moduleScene = App->GetModule<ModuleScene>();
	Scene* loadedScene = moduleScene->GetLoadedScene();

	// Delete a GameObject with the SUPR key
	if (gameObject != loadedScene->GetRoot() && gameObject != loadedScene->GetAmbientLight() &&
		gameObject != loadedScene->GetDirectionalLight())
	{
		if (App->GetModule<ModuleInput>()->GetKey(SDL_SCANCODE_DELETE) == KeyState::DOWN)
		{
			if (gameObject == moduleScene->GetSelectedGameObject() &&
				gameObject != App->GetModule<ModulePlayer>()->GetPlayer())
			{
				moduleScene->SetSelectedGameObject(gameObject->GetParent()); // If a GameObject is destroyed,
																			 // change the focus to its parent
				loadedScene->GetRootQuadtree()->RemoveGameObjectAndChildren(gameObject->GetParent());

				loadedScene->DestroyGameObject(gameObject);

				return;
			}
		}
	}

	char gameObjectLabel[160]; // Label created so ImGui can differentiate the GameObjects
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

		if (gameObject->GetStateOfSelection() == StateOfSelection::CHILD_SELECTED &&
			StateOfSelection::SELECTED == moduleScene->GetSelectedGameObject()->GetStateOfSelection())
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
	if ((ImGui::IsMouseReleased(ImGuiMouseButton_Left) &&
		 ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup)) ||
		(ImGui::IsMouseReleased(ImGuiMouseButton_Right) &&
		 ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup)))
	{
		moduleScene->ChangeSelectedGameObject(gameObject);
	}

	if (ImGui::BeginPopupContextItem("RightClickGameObject", ImGuiPopupFlags_MouseButtonRight))
	{
		if (gameObject->GetComponent(ComponentType::TRANSFORM) != nullptr)
		{
			if (ImGui::MenuItem("Create Empty child"))
			{
				loadedScene->CreateGameObject("Empty GameObject", gameObject);
			}

			if (ImGui::MenuItem("Create camera"))
			{
				GameObject* newCamera = loadedScene->CreateCameraGameObject("Basic Camera", gameObject);
			}

			if (ImGui::MenuItem("Create canvas"))
			{
				GameObject* newCamera = loadedScene->CreateCanvasGameObject("Canvas", gameObject);
			}
			// Create Resource
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
			// Create Light ShortCut
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

		if (gameObject != loadedScene->GetRoot()) // The root can't be neither deleted nor moved up/down
		{
			if (ImGui::MenuItem("Move Up"))
			{
				if (!children.empty() && children[0] != gameObject)
				{
					gameObject->GetParent()->MoveUpChild(gameObject);
				}
			}

			if (ImGui::MenuItem("Move Down"))
			{
				if (!children.empty() && children[children.size() - 1] != gameObject)
				{
					gameObject->GetParent()->MoveDownChild(gameObject);
				}
			}
		}

		if (gameObject != loadedScene->GetRoot() && gameObject != loadedScene->GetAmbientLight() &&
			gameObject != loadedScene->GetDirectionalLight())
		{
			if (ImGui::MenuItem("Delete") && gameObject != App->GetModule<ModulePlayer>()->GetPlayer())
			{
				if (gameObject == moduleScene->GetSelectedGameObject())
				{
					moduleScene->SetSelectedGameObject(gameObject->GetParent()); // If a GameObject is destroyed,
																				 // change the focus to its parent
					moduleScene->RemoveGameObjectAndChildren(gameObject->GetParent());
				}
				moduleScene->RemoveGameObjectAndChildren(gameObject);
				loadedScene->DestroyGameObject(gameObject);

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

	if (gameObject != loadedScene->GetRoot()) // The root cannot be moved around
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
			UID draggedGameObjectID = *(UID*) payload->Data; // Double pointer to keep track correctly
															 // of the UID of the dragged GameObject
			GameObject* draggedGameObject = loadedScene->SearchGameObjectByID(draggedGameObjectID);
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
