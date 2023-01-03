#include "ComponentMesh.h"
#include "3DModels/Mesh.h"
#include "imgui.h"
#include "GameObject/GameObject.h"
#include "Application.h"
#include "ModuleEditor.h"

ComponentMesh::ComponentMesh(bool active, GameObject* owner)
	: Component(ComponentType::MESH, active, owner)
{
}

ComponentMesh::~ComponentMesh()
{
}

void ComponentMesh::Update()
{
	// TODO: Perform Mesh updates (draw...)
}

void ComponentMesh::OnEditorUpdate()
{
	if (ImGui::CollapsingHeader("Mesh"))
	{
		bool active = IsActive();

		if (ImGui::Checkbox("Active", &active))
		{
			active ? Enable() : Disable();
		}
	}
	ImGui::Separator();

	ImGui::TextColored(App->editor->title_color, "Geometry");
	ImGui::TextWrapped("Num Vertices: ");
	ImGui::SameLine();
	ImGui::TextColored(App->editor->text_color, "%d", mesh->numVertices);
	ImGui::TextWrapped("Num Triangles: ");
	ImGui::SameLine();
	ImGui::TextColored(App->editor->text_color, "%d", mesh->numIndexes / 3);
}