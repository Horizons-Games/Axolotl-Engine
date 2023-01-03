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
