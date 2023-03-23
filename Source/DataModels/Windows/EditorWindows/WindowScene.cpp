#pragma warning (disable: 4312)

#include "WindowScene.h"

#include "Application.h"

#include "Modules/ModuleRender.h"
#include "Modules/ModuleCamera.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleInput.h"

#include "Scene/Scene.h"
#include "GameObject/GameObject.h"
#include "Components/ComponentTransform.h"

WindowScene::WindowScene() : EditorWindow("Scene"), texture(0),
	currentWidth(0), currentHeight(0), gizmoCurrentOperation(ImGuizmo::OPERATION::TRANSLATE), 
	gizmoCurrentMode(ImGuizmo::MODE::LOCAL), manipulatedLastFrame(false), useSnap(false), snap(float3(1.f,1.f,1.f))
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_MenuBar;
}

WindowScene::~WindowScene()
{
}

void WindowScene::DrawWindowContents()
{
	ManageResize();

	ImGui::Image((void*)App->renderer->GetRenderedTexture(),
		ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));

	DrawGuizmo();
}

bool WindowScene::isMouseInsideManipulator(float x, float y) const
{
	return x <= viewportBounds[1].x						   &&
		   x >= viewportBounds[1].x - VIEW_MANIPULATE_SIZE &&
		   y >= viewportBounds[0].y						   &&
		   y <= viewportBounds[0].y + VIEW_MANIPULATE_SIZE;
}

void WindowScene::DrawGuizmo()
{
	ImGuiIO& io = ImGui::GetIO();

	ImGui::BeginMenuBar();

	ImGui::Text("Operation type:");
	if (ImGui::RadioButton("Translate", gizmoCurrentOperation == ImGuizmo::OPERATION::TRANSLATE))
	{
		gizmoCurrentOperation = ImGuizmo::OPERATION::TRANSLATE;
	}
	if (ImGui::RadioButton("Rotate", gizmoCurrentOperation == ImGuizmo::OPERATION::ROTATE))
	{
		gizmoCurrentOperation = ImGuizmo::OPERATION::ROTATE;
	}
	if (ImGui::RadioButton("Scale", gizmoCurrentOperation == ImGuizmo::OPERATION::SCALE))
	{
		gizmoCurrentOperation = ImGuizmo::OPERATION::SCALE;
	}

	ImGui::Dummy(ImVec2(5.0f, 0.0f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(5.0f, 0.0f));

	ImGui::Text("Mode:");
	if (ImGui::RadioButton("Local", gizmoCurrentMode == ImGuizmo::LOCAL))
	{
		gizmoCurrentMode = ImGuizmo::MODE::LOCAL;

	}
	if (ImGui::RadioButton("World", gizmoCurrentMode == ImGuizmo::WORLD))
	{
		gizmoCurrentMode = ImGuizmo::MODE::WORLD;

	}

	ImGui::Dummy(ImVec2(5.0f, 0.0f));
	ImGui::Separator();
	ImGui::Dummy(ImVec2(5.0f, 0.0f));

	ImGui::Text("Snap:");
	if (ImGui::RadioButton("", useSnap))
	{
		useSnap = !useSnap;
	};

	if (ImGui::BeginMenu("Value menu"))
	{
		switch(gizmoCurrentOperation)
		{
		case ImGuizmo::OPERATION::TRANSLATE:
			ImGui::Text("X:");
			ImGui::SameLine();
			ImGui::PushItemWidth(50.f);
			ImGui::InputFloat("##XSnap", &snap[0]);

			ImGui::Text("Y:");
			ImGui::SameLine();
			ImGui::PushItemWidth(50.f);
			ImGui::InputFloat("##YSnap", &snap[1]);

			ImGui::Text("Z:");
			ImGui::SameLine();
			ImGui::PushItemWidth(50.f);
			ImGui::InputFloat("##ZSNap", &snap[2]);

			break;

		case ImGuizmo::OPERATION::ROTATE:
			ImGui::Text("Amount:");
			ImGui::SameLine();
			ImGui::PushItemWidth(50.f);
			ImGui::InputFloat("##XSnap", &snap[0]);

			break;

		case ImGuizmo::OPERATION::SCALE:
			ImGui::Text("Amount:");
			ImGui::SameLine();
			ImGui::PushItemWidth(50.f);
			ImGui::InputFloat("##XSnap", &snap[0]);

			break;
		}

		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();

	const GameObject* focusedObject = App->scene->GetSelectedGameObject();
	if (focusedObject != nullptr && focusedObject->GetParent() != nullptr)
	{
		ImVec2 windowPos = ImGui::GetWindowPos();
		float windowWidth = (float)ImGui::GetWindowWidth();
		float windowheight = (float)ImGui::GetWindowHeight();

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(windowPos.x, windowPos.y, windowWidth, windowheight);
		ImGuizmo::SetOrthographic(false);

		float4x4 viewMat = App->camera->GetCamera()->GetViewMatrix().Transposed();
		float4x4 projMat = App->camera->GetCamera()->GetProjectionMatrix().Transposed();

		ComponentTransform* focusedTransform =
			static_cast<ComponentTransform*>(focusedObject->GetComponent(ComponentType::TRANSFORM));

		//Guizmo 3D
		if (static_cast<ComponentTransform*>(focusedObject->GetComponent(ComponentType::TRANSFORM)) != nullptr)
		{
			float4x4 modelMatrix = focusedTransform->GetGlobalMatrix().Transposed();

			ImGuizmo::Manipulate(viewMat.ptr(), projMat.ptr(), (ImGuizmo::OPERATION)gizmoCurrentOperation,
				(ImGuizmo::MODE)gizmoCurrentMode, modelMatrix.ptr(), NULL, useSnap ? &snap[0] : NULL);

			if (ImGuizmo::IsUsing())
			{
				GameObject* parent = focusedObject->GetParent();
				float3 position, scale;
				float4x4 rotation;
				float4x4 inverseParentMatrix = float4x4::identity; //Needs to be identity in case the parent is nulltpr
				float4x4 localMatrix;

				if (parent != nullptr) {
					const ComponentTransform* parentTransform =
						static_cast<ComponentTransform*>(parent->GetComponent(ComponentType::TRANSFORM));

					inverseParentMatrix = parentTransform->GetGlobalMatrix().Inverted();
				}

				localMatrix = inverseParentMatrix * modelMatrix.Transposed();
				localMatrix.Decompose(position, rotation, scale);

				switch (gizmoCurrentOperation)
				{
				case ImGuizmo::OPERATION::TRANSLATE:
					focusedTransform->SetPosition(position);
					break;
				case ImGuizmo::OPERATION::ROTATE:
					focusedTransform->SetRotation(rotation);
					break;
				case ImGuizmo::OPERATION::SCALE:
					focusedTransform->SetScale(scale);
					break;
				}
				focusedTransform->UpdateTransformMatrices();

				for (Component* component : focusedObject->GetComponents())
				{
					if (component->GetType() == ComponentType::LIGHT)
					{
						Scene* scene = App->scene->GetLoadedScene();
						const ComponentLight* light = (ComponentLight*)component;

						switch (light->GetLightType())
						{
						case LightType::DIRECTIONAL:
							scene->RenderDirectionalLight();
							break;
						case LightType::SPOT:
							scene->UpdateSceneSpotLights();
							scene->RenderSpotLights();
							break;
						case LightType::POINT:
							scene->UpdateScenePointLights();
							scene->RenderPointLights();
							break;
						}
					}
				}
			
			}

			float viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
			float viewManipulateTop = ImGui::GetWindowPos().y + VIEW_MANIPULATE_TOP_PADDING;

			ImGuizmo::ViewManipulate(
				viewMat.ptr(),
				App->camera->GetCamera()->GetDistance(
					float3(modelMatrix.Transposed().x, modelMatrix.Transposed().y, modelMatrix.Transposed().z)),
				ImVec2(viewManipulateRight - VIEW_MANIPULATE_SIZE, viewManipulateTop),
				ImVec2(VIEW_MANIPULATE_SIZE, VIEW_MANIPULATE_SIZE),
				0x10101010);

		}
		if (ImGui::IsWindowFocused())
		{
			if (App->input->GetKey(SDL_SCANCODE_Q) == KeyState::DOWN &&
				App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::IDLE)
			{
				gizmoCurrentOperation = ImGuizmo::OPERATION::TRANSLATE;
			}
			if (App->input->GetKey(SDL_SCANCODE_W) == KeyState::DOWN &&
				App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::IDLE)
			{
				gizmoCurrentOperation = ImGuizmo::OPERATION::ROTATE;
			}
			if (App->input->GetKey(SDL_SCANCODE_E) == KeyState::DOWN &&
				App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::IDLE)
			{
				gizmoCurrentOperation = ImGuizmo::OPERATION::SCALE;
			}

			if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) != KeyState::IDLE ||
				App->input->GetKey(SDL_SCANCODE_LALT) != KeyState::IDLE ||
				App->input->GetKey(SDL_SCANCODE_F) != KeyState::IDLE)
			{
				ImGuizmo::Enable(false);
			}
			else
			{
				if (isMouseInsideManipulator(io.MousePos.x, io.MousePos.y))
				{
					manipulatedViewMatrix = viewMat.InverseTransposed();;

					App->camera->GetCamera()->GetFrustum()->SetFrame(
						manipulatedViewMatrix.Col(3).xyz(),  //position
						-manipulatedViewMatrix.Col(2).xyz(), //rotation
						manipulatedViewMatrix.Col(1).xyz()   //scale
					);

					manipulatedLastFrame = true;
				}

				else if (manipulatedLastFrame)
				{
					float3 position, scale;
					Quat rotation;

					App->camera->GetCamera()->SetPosition(manipulatedViewMatrix.Col(3).xyz());

					manipulatedLastFrame = false;
				}

				ImGuizmo::Enable(true);
			}
		}
	}
}

void WindowScene::ManageResize()
{
	auto viewportOffset = ImGui::GetCursorPos(); // include tab bar
	
	availableRegion = ImGui::GetContentRegionAvail();
	bool widthChanged = currentWidth != availableRegion.x;
	bool heightChanged = currentHeight != availableRegion.y;
	if (widthChanged || heightChanged) // window was resized
	{ 
		App->camera->GetCamera()->SetAspectRatio(availableRegion.x / availableRegion.y);
		currentWidth = availableRegion.x;
		currentHeight = availableRegion.y;
	}
	
	auto windowSize = ImGui::GetWindowSize();

	ImVec2 minBounds = ImGui::GetWindowPos();
	minBounds.x += viewportOffset.x;
	minBounds.y += viewportOffset.y;
	
	ImVec2 maxBounds = { minBounds.x + windowSize.x, minBounds.y + windowSize.y};
	
	viewportBounds[0] = { minBounds.x, minBounds.y };
	viewportBounds[1] = { maxBounds.x, maxBounds.y };
}
