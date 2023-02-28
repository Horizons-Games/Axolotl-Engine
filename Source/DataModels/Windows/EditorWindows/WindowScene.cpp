#pragma warning (disable: 4312)

#include "WindowScene.h"

#include "imgui.h"

#include "Application.h"

#include "GameObject/GameObject.h"

#include "Modules/ModuleRender.h"
#include "Modules/ModuleEngineCamera.h"
#include "Modules/ModuleScene.h"

#include "Components/ComponentTransform.h"

WindowScene::WindowScene() : EditorWindow("Scene"), texture(0),
	currentWidth(0), currentHeight(0), gizmoCurrentOperation(ImGuizmo::OPERATION::TRANSLATE), 
	gizmoCurrentMode(ImGuizmo::MODE::WORLD)
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

	ImGui::Dummy(ImVec2(10.0f, 0.0f));

	ImGui::Text("Mode:");
	if (ImGui::RadioButton("Local", gizmoCurrentMode == ImGuizmo::LOCAL))
	{
		gizmoCurrentMode = ImGuizmo::MODE::LOCAL;

	}
	if (ImGui::RadioButton("World", gizmoCurrentMode == ImGuizmo::WORLD))
	{
		gizmoCurrentMode = ImGuizmo::MODE::WORLD;

	}

	ImGui::Dummy(ImVec2(10.0f, 0.0f));
	ImGui::Text("Mouse coords X: %f Y: %f", io.MousePos.x, io.MousePos.y);

	ImGui::EndMenuBar();

	const GameObject* focusedObject = App->scene->GetSelectedGameObject();
	if (focusedObject != nullptr && focusedObject->GetParent() != nullptr)
	{
		ImVec2 windowPos = ImGui::GetWindowPos();

		ImGuizmo::SetDrawlist();
		ImGuizmo::SetRect(windowPos.x, windowPos.y, (float)ImGui::GetWindowWidth(), (float)ImGui::GetWindowHeight());
		ImGuizmo::SetOrthographic(false);

		math::float4x4 viewMat = App->engineCamera->GetViewMatrix().Transposed();
		math::float4x4 projMat = App->engineCamera->GetProjectionMatrix().Transposed();

		ComponentTransform* focusedTransform =
			static_cast<ComponentTransform*>(focusedObject->GetComponent(ComponentType::TRANSFORM));

		math::float4x4 modelMatrix = focusedTransform->GetGlobalMatrix().Transposed();

		ImGuizmo::Manipulate(viewMat.ptr(), projMat.ptr(), (ImGuizmo::OPERATION)gizmoCurrentOperation,
			(ImGuizmo::MODE)gizmoCurrentMode, modelMatrix.ptr());

		if (ImGuizmo::IsUsing())
		{
			GameObject* parent = focusedObject->GetParent();
			float4x4 inverseParentMatrix = float4x4::identity; //Needs to be identity in case the parent is nulltpr
			float4x4 localMatrix;
			float3 transform, scale;
			Quat rotate;

			if (parent != nullptr) {
				const ComponentTransform* parentTransform = 
					static_cast<ComponentTransform*>(parent->GetComponent(ComponentType::TRANSFORM));

				inverseParentMatrix = parentTransform->GetGlobalMatrix().Inverted();
			}

			localMatrix = inverseParentMatrix * modelMatrix.Transposed();
			
			localMatrix.Decompose(transform, rotate, scale);

			switch (gizmoCurrentOperation)
			{
			case ImGuizmo::OPERATION::TRANSLATE:
				focusedTransform->SetPosition(transform);
				break;
			case ImGuizmo::OPERATION::ROTATE:
				focusedTransform->SetRotation(rotate);
				break;
			case ImGuizmo::OPERATION::SCALE:
				focusedTransform->SetScale(scale);
				break;
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
		App->engineCamera->SetAspectRatio(availableRegion.x / availableRegion.y);
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
