#include "WindowInspector.h"

#include "imgui.h"

#include "Application.h"
#include "ModuleRender.h"
#include "ModuleScene.h"

#include "3DModels/Model.h"
#include "GameObject/GameObject.h"
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
	model = App->renderer->GetModel(0);
	GameObject* currentGameObject = App->scene->GetSelectedGameObject();
	if (App->renderer->AnyModelLoaded() && model.lock()) //checks the model exists
	{
		bool enable = currentGameObject->IsEnabled();
		ImGui::Checkbox("Enable", &enable);

		if (enable)
		{
			currentGameObject->Enable();
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

		DrawTransformationTable(currentGameObject);

		DrawBoundingBoxTable(currentGameObject);
		/*
		DrawGeometryTable();

		ImGui::Separator();

		DrawTextureTable();
		*/

		ComponentCamera* camera = (ComponentCamera*)currentGameObject->GetComponent(ComponentType::CAMERA);
		if (camera != nullptr) DrawCameraTable(camera);
	}
}

void WindowInspector::DrawTransformationTable(GameObject* selected)
{
	//float3 translation = model.lock()->GetTranslation();
	//float3 scale = model.lock()->GetScale();
	//float3 rotation = model.lock()->GetRotationF3();

	if (App->scene->GetRoot() == selected) // The root must not be moved through the inspector
		return;

	ComponentTransform* transform = (ComponentTransform*)selected->GetComponent(ComponentType::TRANSFORM);

	float3 translation = transform->GetPosition();
	float3 rotation = RadToDeg(transform->GetRotation().ToEulerXYZ());
	float3 scale = transform->GetScale();

	ImGui::Text("TRANSFORM");
	ImGui::Dummy(ImVec2(0.0f, 2.5f));

	if (ImGui::BeginTable("TransformTable", 2))
	{
		ImGui::TableNextColumn();
		ImGui::Text("Translation"); ImGui::SameLine();

		ImGui::TableNextColumn();
		ImGui::Text("x:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##XTrans", &translation.x, 0.01f,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min()
		); ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##YTrans", &translation.y, 0.01f,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min()
		); ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##ZTrans", &translation.z, 0.01f,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min()
		); ImGui::PopStyleVar();

		ImGui::TableNextColumn();
		ImGui::Text("Rotation"); ImGui::SameLine();

		ImGui::TableNextColumn();
		ImGui::Text("x:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##XRot", &rotation.x, 0.05f,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%0.3f");
		ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##YRot", &rotation.y, 0.05f,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%0.3f");
		ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##ZRot", &rotation.z, 0.05f,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%0.3f");
		ImGui::PopStyleVar();

		ImGui::TableNextColumn();
		ImGui::Text("Scale"); ImGui::SameLine();

		ImGui::TableNextColumn();
		ImGui::Text("x:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##XScale", &scale.x, 0.01f,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min()
		); ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##YScale", &scale.y, 0.01f,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min()
		); ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##ZScale", &scale.z, 0.01f,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min()
		); ImGui::PopStyleVar();

		//model.lock()->SetTranslation(translation);
		//model.lock()->SetRotation(rotation);
		//model.lock()->SetScale(scale);

		transform->SetPosition(translation);
		transform->SetRotation(rotation);
		transform->SetScale(scale);

		ImGui::EndTable();
		ImGui::Separator();
	}
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