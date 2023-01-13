#include "WindowInspector.h"

#include "imgui.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScene.h"

#include "3DModels/Model.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentTransform.h"
#include "Components/ComponentCamera.h"
#include "Components/ComponentBoundingBoxes.h"

WindowInspector::WindowInspector() : EditorWindow("Inspector")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;
}

WindowInspector::~WindowInspector()
{
}

void WindowInspector::DrawWindowContents()
{
	GameObject* currentGameObject = App->scene->GetSelectedGameObject();

	bool enable = currentGameObject->IsEnabled();
	ImGui::Checkbox("Enable", &enable);

	(enable) ? currentGameObject->Enable() : currentGameObject->Disable();

	ImGui::SameLine();

	if (currentGameObject->GetParent() == nullptr) // Keep the word Scene in the scene root
	{
		char* name = (char*)currentGameObject->GetName();
		if (ImGui::InputText("##GameObject", name, 24))
		{
			std::string scene = " Scene";
			std::string sceneName = name + scene;
			currentGameObject->SetName(sceneName.c_str());
		}
		else
		{
			currentGameObject->Disable();
		}

		ImGui::SameLine();

		if (currentGameObject->GetParent() == nullptr) // Keep the word Scene in the root
		{
			char* name = (char*)currentGameObject->GetName();
			if (ImGui::InputText("##GameObject", name, 24))
			{
				std::string scene = " Scene";
				std::string sceneName = name + scene;
				currentGameObject->SetName(sceneName.c_str());
			}
		}

		else
		{
			char* name = (char*)currentGameObject->GetName();
			ImGui::InputText("##GameObject", name, 24);
		}

		ImGui::Separator();

		/*DrawTransformationTable(currentGameObject);

		DrawBoundingBoxTable(currentGameObject);

		DrawGeometryTable();

		ImGui::Separator();

		DrawTextureTable();

		ComponentCamera* camera = (ComponentCamera*)currentGameObject->GetComponent(ComponentType::CAMERA);
		if (camera != nullptr) DrawCameraTable(camera);
		*/
	}

	else
	{
		char* name = (char*)currentGameObject->GetName();
		ImGui::InputText("##GameObject", name, 24);
	}

	ImGui::Separator();

	for (Component* component : currentGameObject->GetComponents())
	{
		component->Display();
	}

	/*
	DrawGeometryTable();

	DrawTextureTable();
	*/
}

void WindowInspector::DrawBoundingBoxTable(GameObject* selected)
{
	
	if (App->scene->GetRoot() == selected) // The root must not have BoundingBox
		return;

	ComponentBoundingBoxes* boundingBox = (ComponentBoundingBoxes*)selected->GetComponent(ComponentType::BOUNDINGBOX);

	bool drawBox = boundingBox->isDrawBoundingBoxes();

	ImGui::Text("BOUNDING BOXES");
	ImGui::Dummy(ImVec2(0.0f, 2.5f));
	if (ImGui::BeginTable("BoundingTable", 2))
	{
		ImGui::TableNextColumn();
		ImGui::Text("Draw Bounding Box"); ImGui::SameLine();
		if (ImGui::Checkbox("", &drawBox))
		{
			boundingBox->setDrawBoundingBoxes(drawBox);
		}
		ImGui::EndTable();
		ImGui::Separator();
	}
	
}

void WindowInspector::DrawGeometryTable()
{
	ImGui::Text("GEOMETRY");
	ImGui::Dummy(ImVec2(0.0f, 2.5f));
	if (ImGui::BeginTable("GeometryTable1", 2))
	{
		ImGui::TableNextColumn();
		ImGui::Text("Number of vertices: ");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", model.lock()->GetNumVertices());
		ImGui::TableNextColumn();
		ImGui::Text("Number of triangles: ");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", model.lock()->GetNumTriangles());

		ImGui::EndTable();
		ImGui::Separator();
	}
}

void WindowInspector::DrawTextureTable()
{
	ImGui::Text("TEXTURE");
	ImGui::Dummy(ImVec2(0.0f, 2.5f));
	if (ImGui::BeginTable("TextureTable1", 2))
	{
		ImGui::TableNextColumn();
		ImGui::Text("Height: ");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", model.lock()->GetTextureHeight(0));
		ImGui::TableNextColumn();
		ImGui::Text("Width: ");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", model.lock()->GetTextureWidth(0));

		ImGui::EndTable();
		ImGui::Separator();
	}

	ImGui::Image((void*)model.lock()->GetTextureID(0), ImVec2(100.0f, 100.0f), ImVec2(0, 1), ImVec2(1, 0));
}


void WindowInspector::DrawCameraTable(ComponentCamera* camera)
{

	bool draw = camera->IsDrawFrustum();
	const char* listbox_items[] = { "Basic Frustum", "Offset Frustum", "No Frustum" };
	int currentFrustum = camera->GetFrustumMode();
	float currentOffset = camera->GetFrustumOffset();


	ImGui::Text("CAMERA");
	ImGui::Dummy(ImVec2(0.0f, 2.5f));

	if (ImGui::BeginTable("CameraComponentTable", 2))
	{
		ImGui::TableNextColumn();
		ImGui::Text("Draw Frustum"); ImGui::SameLine();
		if (ImGui::Checkbox("", &draw))
		{
			camera->SetDrawFrustum(draw);
		}

		if (ImGui::ListBox("Frustum Mode\n(single select)", &currentFrustum, listbox_items, IM_ARRAYSIZE(listbox_items), 3))
		{
			camera->SetFrustumMode(currentFrustum);
		}

		if (ImGui::SliderFloat("Frustum Offset", &currentOffset, -2.f, 2.f, "%.0f", ImGuiSliderFlags_AlwaysClamp)) {
			camera->SetFrustumOffset(currentOffset);
		}

		ImGui::EndTable();
		ImGui::Separator();
	}
}