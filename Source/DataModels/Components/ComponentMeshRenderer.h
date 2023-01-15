#pragma once

#include "Components/Component.h"
#include "FileSystem/UniqueID.h"

#include <memory>
#include <DataModels/Resources/ResourceMaterial.h>

class ResourceMesh;
class ResourceTexture;

class ComponentMeshRenderer : public Component
{
public:
	ComponentMeshRenderer(const bool active, GameObject* owner);
	~ComponentMeshRenderer() override;

	void Update() override;

	void Draw();
	void Display();

	void SetMesh(ResourceMesh* meshUID);

	ResourceMesh* GetMesh() const;
	ResourceTexture* GetTexture() const; // To Material Component

	void SetMaterialComponent(ComponentMaterial* newMaterial);

private:
	bool IsMeshLoaded();

	std::weak_ptr<ResourceMesh> mesh;
	
	ComponentMaterial* material;
};

inline ResourceMesh* ComponentMeshRenderer::GetMesh() const
{
	return mesh.get();
}

inline ResourceTexture* ComponentMeshRenderer::GetTexture() const
{
	return texture.get();
}

inline bool ComponentMeshRenderer::IsMeshLoaded()
{
	return !mesh.expired();
}