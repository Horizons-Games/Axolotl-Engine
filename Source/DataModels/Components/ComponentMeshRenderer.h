#pragma once

#include "Components/Component.h"
#include "FileSystem/UniqueID.h"

#include <memory>

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
	void SetTexture(ResourceTexture* textureUID); // To Material Component

	ResourceMesh* GetMesh() const;
	ResourceTexture* GetTexture() const; // To Material Component

private:
	bool IsMeshLoaded();

	std::shared_ptr<ResourceMesh> mesh = nullptr;
	std::shared_ptr<ResourceTexture> texture = nullptr; // To Material Component
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
	return (mesh != nullptr);
}