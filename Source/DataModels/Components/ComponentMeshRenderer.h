#pragma once

#include <memory>

#include "Components/Component.h"

class ResourceMesh;
class ResourceTexture;

class ComponentMeshRenderer : public Component
{
public:
	ComponentMeshRenderer(const bool active, GameObject* owner, ResourceMesh* mesh, ResourceTexture* texture);
	~ComponentMeshRenderer() override;

	void Update() override;

	void Draw();

private:
	void SetMesh(ResourceMesh* mesh);
	void SetTexture(ResourceTexture* texture);

	ResourceMesh* mesh = nullptr;
	ResourceTexture* texture = nullptr;
};

inline void ComponentMeshRenderer::SetMesh(ResourceMesh* mesh)
{
	this->mesh = mesh;
}

inline void ComponentMeshRenderer::SetTexture(ResourceTexture* texture)
{
	this->texture = texture;
}