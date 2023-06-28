#include "StdAfx.h"

#include "WindowScene.h"

#include "Application.h"

#include "Modules/ModuleCamera.h"
#include "Modules/ModuleInput.h"
#include "Modules/ModuleRender.h"
#include "Modules/ModuleScene.h"
#include "Modules/ModuleUI.h"

#include "Components/ComponentTransform.h"
#include "Components/UI/ComponentTransform2D.h"
#include "Components/UI/ComponentCanvas.h"
#include "GameObject/GameObject.h"
#include "Scene/Scene.h"
#include "Camera/Camera.h"


WindowScene::WindowScene() :
	EditorWindow("Scene"),
	texture(0),
	currentWidth(0),
	currentHeight(0),
	gizmoCurrentOperation(ImGuizmo::OPERATION::TRANSLATE),
	gizmoCurrentMode(ImGuizmo::MODE::LOCAL),
	manipulatedLastFrame(false),
	useSnap(false),
	snap(float3(1.f, 1.f, 1.f))
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_MenuBar;
}

WindowScene::~WindowScene()
{
}

void WindowScene::DrawWindowContents()
{
	ManageResize();

	ImGui::Image(reinterpret_cast<void*>(static_cast<uintptr_t>(App->GetModule<ModuleRender>()->GetRenderedTexture())),
				 ImGui::GetContentRegionAvail(),
				 ImVec2(0, 1),
				 ImVec2(1, 0));
	if (App->GetPlayState() != Application::PlayState::STOPPED)
	{
		DrawGuizmo();
	}
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
		switch (gizmoCurrentOperation)
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

	const GameObject* focusedObject = App->GetModule<ModuleScene>()->GetSelectedGameObject();
	if (focusedObject != nullptr && focusedObject->GetParent() != nullptr)
	{
		ImVec2 windowPos = ImGui::GetWindowPos();
		float windowWidth = static_cast<float>(ImGui::GetWindowWidth());
		float windowheight = static_cast<float>(ImGui::GetWindowHeight());

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(windowPos.x, windowPos.y, windowWidth, windowheight);

		ModuleCamera* camera = App->GetModule<ModuleCamera>();
		ModuleInput* input = App->GetModule<ModuleInput>();

		float4x4 viewMat = float4x4::identity;

		ComponentTransform* focusedTransform = focusedObject->GetComponent<ComponentTransform>();

		// Guizmo 3D
		if (focusedTransform != nullptr)
		{
			ImGuizmo::SetOrthographic(false);
			viewMat = camera->GetCamera()->GetViewMatrix().Transposed();
			float4x4 projMat = camera->GetCamera()->GetProjectionMatrix().Transposed();
			float4x4 modelMatrix = focusedTransform->GetGlobalMatrix().Transposed();

			ImGuizmo::Manipulate(viewMat.ptr(),
								 projMat.ptr(),
								 (ImGuizmo::OPERATION) gizmoCurrentOperation,
								 (ImGuizmo::MODE) gizmoCurrentMode,
								 modelMatrix.ptr(),
								 NULL,
								 useSnap ? &snap[0] : nullptr);

			ImGuizmo::Enable(IsFocused());

			if (ImGuizmo::IsUsing())
			{
				const GameObject* parent = focusedObject->GetParent();
				float3 position, scale;
				Quat rotation;
				float4x4 inverseParentMatrix = float4x4::identity; // Needs to be identity in case the parent is nulltpr
				float4x4 localMatrix;

				if (parent != nullptr)
				{
					const ComponentTransform* parentTransform = parent->GetComponent<ComponentTransform>();

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
			}

			float viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
			float viewManipulateTop = ImGui::GetWindowPos().y + VIEW_MANIPULATE_TOP_PADDING;

			ImGuizmo::ViewManipulate(
				viewMat.ptr(),
				camera->GetCamera()->GetDistance(
					float3(modelMatrix.Transposed().x, modelMatrix.Transposed().y, modelMatrix.Transposed().z)),
				ImVec2(viewManipulateRight - VIEW_MANIPULATE_SIZE, viewManipulateTop),
				ImVec2(VIEW_MANIPULATE_SIZE, VIEW_MANIPULATE_SIZE),
				0x10101010);
		}
		//Guizmo 2D
		else
		{
			ComponentTransform2D* focusedTransform2D = focusedObject->GetComponent<ComponentTransform2D>();
			ImGuizmo::SetOrthographic(true);
			float4x4 projMat = camera->GetOrthoProjectionMatrix().Transposed();
			float4x4 modelMatrix = focusedTransform2D->GetGlobalMatrix().Transposed();

			ComponentCanvas* canvas = focusedTransform2D->WhichCanvasContainsMe();
			if (canvas)
			{
				canvas->RecalculateSizeAndScreenFactor();
				float factor = canvas->GetScreenFactor();
				viewMat = viewMat * float4x4::Scale(factor, factor, factor);
			}

			ImGuizmo::Manipulate(viewMat.ptr(), projMat.ptr(), ImGuizmo::OPERATION::TRANSLATE,
				ImGuizmo::MODE::WORLD, modelMatrix.ptr(), NULL, useSnap ? &snap[0] : NULL);

			if (ImGuizmo::IsUsing())
			{
				GameObject* parent = focusedObject->GetParent();
				float3 position, scale;
				Quat rotation;
				
				modelMatrix.Transposed().Decompose(position, rotation, scale);

				switch (gizmoCurrentOperation)
				{
				case ImGuizmo::OPERATION::TRANSLATE:
					position.z = focusedTransform2D->GetLocalMatrix().z;
					focusedTransform2D->SetPosition(position);
					break;
				case ImGuizmo::OPERATION::SCALE:
					focusedTransform2D->SetScale(scale);
					break;
				}

				focusedTransform2D->CalculateMatrices();
			}
		}

		if (IsFocused())
		{
			if (input->GetKey(SDL_SCANCODE_Q) == KeyState::DOWN &&
				input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::IDLE)
			{
				gizmoCurrentOperation = ImGuizmo::OPERATION::TRANSLATE;
			}
			if (input->GetKey(SDL_SCANCODE_W) == KeyState::DOWN &&
				input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::IDLE)
			{
				gizmoCurrentOperation = ImGuizmo::OPERATION::ROTATE;
			}
			if (input->GetKey(SDL_SCANCODE_E) == KeyState::DOWN &&
				input->GetMouseButton(SDL_BUTTON_RIGHT) == KeyState::IDLE)
			{
				gizmoCurrentOperation = ImGuizmo::OPERATION::SCALE;
			}

			if (input->GetMouseButton(SDL_BUTTON_RIGHT) != KeyState::IDLE ||
				input->GetKey(SDL_SCANCODE_LALT) != KeyState::IDLE || input->GetKey(SDL_SCANCODE_F) != KeyState::IDLE)
			{
				ImGuizmo::Enable(false);
			}
			else
			{
				if (IsMouseInsideManipulator(io.MousePos.x, io.MousePos.y))
				{
					manipulatedViewMatrix = viewMat.InverseTransposed();

					camera->GetCamera()->GetFrustum()->SetFrame(manipulatedViewMatrix.Col(3).xyz(),	 // position
																-manipulatedViewMatrix.Col(2).xyz(), // rotation
																manipulatedViewMatrix.Col(1).xyz()	 // scale
					);

					manipulatedLastFrame = true;
				}

				else if (manipulatedLastFrame)
				{
					float3 position, scale;
					Quat rotation;

					camera->GetCamera()->SetPosition(manipulatedViewMatrix.Col(3).xyz());

					manipulatedLastFrame = false;
				}

				ImGuizmo::Enable(true);
			}
		}
	}
}

void WindowScene::ManageResize()
{
	ImVec2 viewportOffset = ImGui::GetCursorPos(); // include tab bar

	availableRegion = ImGui::GetContentRegionAvail();
	bool widthChanged = currentWidth != availableRegion.x;
	bool heightChanged = currentHeight != availableRegion.y;
	ModuleCamera* camera = App->GetModule<ModuleCamera>();

	if (widthChanged || heightChanged) // window was resized
	{
		camera->GetCamera()->SetAspectRatio(availableRegion.x / availableRegion.y);
		camera->RecalculateOrthoProjectionMatrix();
		currentWidth = availableRegion.x;
		currentHeight = availableRegion.y;
		App->GetModule<ModuleUI>()->RecalculateCanvasSizeAndScreenFactor();
	}

	ImVec2 windowSize = ImGui::GetWindowSize();

	ImVec2 minBounds = ImGui::GetWindowPos();
	minBounds.x += viewportOffset.x;
	minBounds.y += viewportOffset.y;

	ImVec2 maxBounds = { minBounds.x + windowSize.x, minBounds.y + windowSize.y };

	viewportBounds[0] = { minBounds.x, minBounds.y };
	viewportBounds[1] = { maxBounds.x, maxBounds.y };
}
