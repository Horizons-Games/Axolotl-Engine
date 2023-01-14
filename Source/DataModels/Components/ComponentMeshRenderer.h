#pragma once

#include "Components/Component.h"
#include "FileSystem/UniqueID.h"

#include <memory>

class ResourceMesh;
class ResourceTexture;

class ComponentMeshRenderer : public Component
{
public:
	ComponentMeshRenderer(const bool active, GameObject* owner, UID meshUID, UID textureUID);
	~ComponentMeshRenderer() override;

	bool Init();
	void Update() override;

	void Draw();
	void Display();

	void SetMeshUID(UID& meshUID);
	void SetTextureUID(UID& textureUID);

	const UID& GetMeshUID() const;
	const UID& GetTextureUID() const;

private:
	void LoadMesh();
	void LoadTexture();
	bool IsMeshLoaded();

	std::shared_ptr<ResourceMesh> mesh = nullptr;
	std::shared_ptr<ResourceTexture> texture = nullptr;
	
	UID meshUID = 0;
	UID textureUID = 0;
};

inline const UID& ComponentMeshRenderer::GetMeshUID() const
{
	return meshUID;
}

inline const UID& ComponentMeshRenderer::GetTextureUID() const
{
	return textureUID;
}

inline bool ComponentMeshRenderer::IsMeshLoaded()
{
	return (mesh != nullptr);
}