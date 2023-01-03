#include "WindowInspector.h"

#include "imgui.h"
#include "GameObject/GameObject.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleEditor.h"
#include "Components/Component.h"

#include "3DModels/Model.h"

WindowInspector::WindowInspector() : EditorWindow("Inspector")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;
}

WindowInspector::~WindowInspector()
{
}

void WindowInspector::DrawWindowContents()
{
	modelInspector = App->renderer->GetModel(0);
	if (App->renderer->AnyModelLoaded() && modelInspector.lock()) //checks the model exists
	{
		static bool check = true;
		ImGui::Checkbox("Enable", &check);
		GameObject* selected = App->editor->Selected_Object;
		if (selected != nullptr)
		{
			ImGui::TextUnformatted("Id:");
			ImGui::SameLine();
			ImGui::TextColored(App->editor->text_color, "%llu", selected->GetName());

			char name[100];
			sprintf_s(name, 100, "%s", selected->name.c_str());
			if (ImGui::InputText("Name", name, 100))
			{
				selected->name = name;
			}

			bool active = selected->GetActive();

			if (ImGui::Checkbox("Active##game_object", &active))
			{
				if (active)
				{
					selected->Enable();
				}
				else
				{
					selected->Disable();
				}
			}

			ImGui::Separator();
					
		}

		ImGui::SameLine();

		char str0[128] = "gameobjectName";
		ImGui::InputText("GameobjectName", str0, IM_ARRAYSIZE(str0));

		ImGui::Separator();

		DrawTransformationTable();

		ImGui::Separator();

		DrawGeometryTable();

		ImGui::Separator();

		DrawTextureTable();
	}
}

void WindowInspector::DrawTransformationTable()
{
	float3 translation = modelInspector.lock()->GetTranslation();
	float3 scale = modelInspector.lock()->GetScale();
	float3 rotation = modelInspector.lock()->GetRotationF3();

	ImGui::Text("TRANSFORMATION");
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
		ImGui::DragFloat("##XRot", &rotation.x, 0.01f,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%0.3f");
		ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##YRot", &rotation.y, 0.01f,
			std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%0.3f");
		ImGui::PopStyleVar(); ImGui::SameLine();

		ImGui::Text("z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
		ImGui::DragFloat("##ZRot", &rotation.z, 0.01f,
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

		modelInspector.lock()->SetTranslation(translation);
		modelInspector.lock()->SetRotation(rotation);
		modelInspector.lock()->SetScale(scale);

		ImGui::EndTable();
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
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", modelInspector.lock()->GetNumVertices());
		ImGui::TableNextColumn();
		ImGui::Text("Number of triangles: ");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", modelInspector.lock()->GetNumTriangles());

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
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", modelInspector.lock()->GetTextureHeight(0));
		ImGui::TableNextColumn();
		ImGui::Text("Width: ");
		ImGui::TableNextColumn();
		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", modelInspector.lock()->GetTextureWidth(0));

		ImGui::EndTable();
	}
	ImGui::Image((void*)modelInspector.lock()->GetTextureID(0), ImVec2(100.0f, 100.0f), ImVec2(0, 1), ImVec2(1, 0));
}
