#include "StdAfx.h"

#include "WindowHierarchy.h"

#include "Application.h"
#include "ModuleEditor.h"
#include "ModuleInput.h"
#include "ModulePlayer.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"
#include "Windows/EditorWindows/WindowScene.h"

#include "DataModels/Components/ComponentTransform.h"

#include "DataStructures/Quadtree.h"

static ImVec4 grey = ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
static ImVec4 white = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

WindowHierarchy::WindowHierarchy() : EditorWindow("Hierarchy"), objectHasBeenCut(false), lastSelectedGameObject()
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;
}

WindowHierarchy::~WindowHierarchy()
{
}

void WindowHierarchy::DrawWindowContents()
{
	DrawSearchBar();

	// if filteredObjects contains a "null" UID, means no object was found
	if (filteredObjects.count(0U) != 0)
	{
		ImGui::TextUnformatted("No Game Object with that name found in the scene");
		return;
	}

	ImGui::Separator();

	GameObject* root = App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot();
	if (root && !App->GetModule<ModuleScene>()->IsLoading())
	{
		DrawRecursiveHierarchy(root);
	}

	if (IsFocused() || App->GetModule<ModuleEditor>()->GetScene()->IsFocused())
	{
		ProcessInput();
	}

	lastSelectedGameObject = App->GetModule<ModuleScene>()->GetSelectedGameObject();
}

WindowHierarchy::DrawHierarchyResultCode WindowHierarchy::DrawRecursiveHierarchy(GameObject* gameObject)
{
	assert(gameObject);

	if (!IsFiltered(gameObject))
	{
		return DrawHierarchyResultCode::ObjectNotFiltered;
	}

	ModuleScene* moduleScene = App->GetModule<ModuleScene>();
	ModulePlayer* modulePlayer = App->GetModule<ModulePlayer>();
	Scene* loadedScene = moduleScene->GetLoadedScene();

	char gameObjectLabel[160]; // Label created so ImGui can differentiate the GameObjects
							   // that have the same name in the hierarchy window
	sprintf_s(gameObjectLabel, "%s###%p", gameObject->GetName().c_str(), gameObject);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

	GameObject::GameObjectView children = gameObject->GetChildren();
	if (gameObject == loadedScene->GetRoot())
	{
		flags |= ImGuiTreeNodeFlags_DefaultOpen;
	}
	else if (children.empty() || std::none_of(std::begin(children),
											  std::end(children),
											  std::bind(&WindowHierarchy::IsFiltered, this, std::placeholders::_1)))
	{
		flags |= ImGuiTreeNodeFlags_Leaf;
	}

	bool ancestorOfSelectedObject = gameObject->GetStateOfSelection() == StateOfSelection::CHILD_SELECTED &&
									lastSelectedGameObject != App->GetModule<ModuleScene>()->GetSelectedGameObject();
	bool objectIsFiltered = !filteredObjects.empty();
	if (ancestorOfSelectedObject || objectIsFiltered)
	{
		ImGui::SetNextItemOpen(true);
	}

	if (gameObject == moduleScene->GetSelectedGameObject())
	{
		flags |= ImGuiTreeNodeFlags_Selected;
	}

	ImGui::PushStyleColor(0, (gameObject->IsEnabled() && gameObject->IsActive()) ? white : grey);
	bool nodeDrawn = ImGui::TreeNodeEx(gameObjectLabel, flags);
	ImGui::PopStyleColor();

	ImGui::PushID(gameObjectLabel);
	if ((ImGui::IsMouseReleased(ImGuiMouseButton_Left) || ImGui::IsMouseReleased(ImGuiMouseButton_Right)) &&
		ImGui::IsItemHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup))
	{
		moduleScene->SetSelectedGameObject(gameObject);
	}

	if (ImGui::BeginPopupContextItem("RightClickGameObject", ImGuiPopupFlags_MouseButtonRight))
	{
		if (gameObject->GetComponentInternal<ComponentTransform>() != nullptr)
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
			Create3DObjectMenu(gameObject);

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
				if (ImGui::BeginMenu("Area Light"))
				{
					if (ImGui::MenuItem("Sphere"))
					{
						loadedScene->CreateLightGameObject("Area Light", gameObject, LightType::AREA, AreaType::SPHERE);
					}
					if (ImGui::MenuItem("Tube"))
					{
						loadedScene->CreateLightGameObject("Area Light", gameObject, LightType::AREA, AreaType::TUBE);
					}
					ImGui::EndMenu();
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

		MoveObjectMenu(gameObject);

		if (IsModifiable(gameObject) && ImGui::MenuItem("Delete") && gameObject != modulePlayer->GetPlayer())
		{
			DeleteGameObject(gameObject);
			ImGui::EndPopup();
			ImGui::PopID();
			if (nodeDrawn) // If the parent node is correctly drawn, draw its children
			{
				ImGui::TreePop();
			}
			return DrawHierarchyResultCode::HierarchyChanged;
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
			GameObject* draggedGameObject = loadedScene->SearchGameObjectByID(draggedGameObjectID);

			if (draggedGameObject)
			{
				GameObject* parentGameObject = draggedGameObject->GetParent();

				GameObject* selectedGameObject = moduleScene->GetSelectedGameObject();
				if (selectedGameObject && selectedGameObject->GetParent())
				{
					std::list<GameObject*> listSGO = selectedGameObject->GetAllDescendants();
					bool actualParentSelected =
						std::find(std::begin(listSGO), std::end(listSGO), parentGameObject) != std::end(listSGO);
					bool newParentSelected =
						std::find(std::begin(listSGO), std::end(listSGO), gameObject) != std::end(listSGO);

					if (actualParentSelected && !newParentSelected)
					{
						moduleScene->AddGameObjectAndChildren(draggedGameObject);
					}
					else if (!actualParentSelected && newParentSelected)
					{
						moduleScene->RemoveGameObjectAndChildren(draggedGameObject);
					}
				}

				draggedGameObject->SetParent(gameObject);
				ImGui::EndDragDropTarget();
				if (nodeDrawn)
				{
					ImGui::TreePop();
				}
				return DrawHierarchyResultCode::HierarchyChanged;
			}
		}

		ImGui::EndDragDropTarget();
	}

	if (nodeDrawn) // If the parent node is correctly drawn, draw its children
	{
		for (GameObject* child : children)
		{
			if (DrawRecursiveHierarchy(child) == DrawHierarchyResultCode::HierarchyChanged)
			{
				ImGui::TreePop();
				return DrawHierarchyResultCode::HierarchyChanged;
			}
		}
		ImGui::TreePop();
	}

	return DrawHierarchyResultCode::Success;
}

void WindowHierarchy::DrawSearchBar()
{
	// use same buffer size as in Inspector
	static char name[32] = "";
	if (ImGui::InputTextWithHint("##HierarchySearchBar", "Search for a Game Object...", name, 32))
	{
		SetUpFilter(name);
	}
}

void WindowHierarchy::ProcessInput()
{
	ModuleInput* input = App->GetModule<ModuleInput>();

	if (SDL_ShowCursor(SDL_QUERY) && input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::REPEAT ||
		input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::DOWN)
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

	if (input->GetKey(SDL_SCANCODE_DELETE) == KeyState::DOWN)
	{
		DeleteGameObject(App->GetModule<ModuleScene>()->GetSelectedGameObject());
	}
}

void WindowHierarchy::SetUpFilter(const std::string& nameFilter)
{
	filteredObjects.clear();
	if (nameFilter.empty())
	{
		return;
	}
	std::list<GameObject*> entireHierarchy =
		App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot()->GetAllDescendants();

	std::string nameFilterToLower;
	std::transform(std::begin(nameFilter), std::end(nameFilter), std::back_inserter(nameFilterToLower), ::tolower);

	auto objectsWithName = entireHierarchy | std::views::filter(
												 [&nameFilterToLower](const GameObject* gameObject)
												 {
													 std::string gameObjectName = gameObject->GetName();
													 std::transform(std::begin(gameObjectName),
																	std::end(gameObjectName),
																	std::begin(gameObjectName),
																	::tolower);
													 return gameObjectName.find(nameFilterToLower) != std::string::npos;
												 });
	for (const GameObject* gameObject : objectsWithName)
	{
		filteredObjects.insert(gameObject->GetUID());
		const GameObject* ancestor = gameObject->GetParent();
		while (ancestor != nullptr)
		{
			filteredObjects.insert(ancestor->GetUID());
			ancestor = ancestor->GetParent();
		}
	}
	// Object not found, insert "null" UID to differentiate from no filter case
	if (filteredObjects.empty())
	{
		filteredObjects.insert(0U);
	}
}

bool WindowHierarchy::IsFiltered(const GameObject* gameObject) const
{
	if (filteredObjects.empty())
	{
		return true;
	}
	return filteredObjects.count(gameObject->GetUID()) != 0;
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

	if (ImGui::MenuItem("Create Text 2D"))
	{
		loadedScene->CreateUIGameObject("new Text 2D", gameObject, ComponentType::TEXT2D);
	}

	if (ImGui::MenuItem("Create Slider"))
	{
		loadedScene->CreateUIGameObject("new Slider", gameObject, ComponentType::SLIDER);
	}
}

void WindowHierarchy::MoveObjectMenu(GameObject* gameObject)
{
	// The root can't be neither deleted nor moved up/down
	if (gameObject != App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot())
	{
		if (ImGui::MenuItem("Move Up"))
		{
			gameObject->GetParent()->MoveUpChild(gameObject);
		}

		if (ImGui::MenuItem("Move Down"))
		{
			gameObject->GetParent()->MoveDownChild(gameObject);
		}
	}
}

void WindowHierarchy::Create3DObjectMenu(GameObject* gameObject)
{
	Scene* loadedScene = App->GetModule<ModuleScene>()->GetLoadedScene();

	// Create Resource
	if (ImGui::BeginMenu("Create 3D object"))
	{
		if (ImGui::MenuItem("Cube"))
		{
			loadedScene->Create3DGameObject("Cube", gameObject, Premade3D::CUBE);
		}
		if (ImGui::MenuItem("Sphere"))
		{
			loadedScene->Create3DGameObject("Sphere", gameObject, Premade3D::SPHERE);
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

	return gameObject != loadedScene->GetRoot() && gameObject != loadedScene->GetDirectionalLight();
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
		loadedScene->GetRootQuadtree()->RemoveGameObjectAndChildren(gameObject->GetParent());
	}
	App->GetModule<ModuleEditor>()->RefreshInspector();
	loadedScene->DestroyGameObject(gameObject);
}

void WindowHierarchy::CopyAnObject()
{
	GameObject* selectedGameObject = App->GetModule<ModuleScene>()->GetSelectedGameObject();

	if (IsModifiable(selectedGameObject))
	{
		copyObject = std::make_unique<GameObject>(*selectedGameObject);
		objectHasBeenCut = false;
	}
}

void WindowHierarchy::PasteAnObject()
{
	if (copyObject)
	{
		const ModuleScene* moduleScene = App->GetModule<ModuleScene>();
		Scene* loadedScene = moduleScene->GetLoadedScene();
		GameObject* selectedGameObject = moduleScene->GetSelectedGameObject();

		GameObject* parent = selectedGameObject->GetParent();
		if (parent == nullptr)
		{
			parent = loadedScene->GetRoot();
		}
		loadedScene->DuplicateGameObject(copyObject->GetName(), copyObject.get(), parent);

		if (objectHasBeenCut)
		{
			copyObject = nullptr;
		}
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
		objectHasBeenCut = true;
	}
}

void WindowHierarchy::DuplicateAnObject()
{
	ModuleScene* moduleScene = App->GetModule<ModuleScene>();
	Scene* loadedScene = moduleScene->GetLoadedScene();
	GameObject* selectedGameObject = moduleScene->GetSelectedGameObject();

	if (selectedGameObject && IsModifiable(selectedGameObject))
	{
		loadedScene->DuplicateGameObject(
			selectedGameObject->GetName(), selectedGameObject, selectedGameObject->GetParent());
	}
}
