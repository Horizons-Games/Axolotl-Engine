#include "ComponentMesh.h"

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

void ComponentMesh::Draw()
{
}

void ComponentMesh::Display()
{
}