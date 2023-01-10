#pragma once

#include "Components/Component.h"
#include "FileSystem/UniqueID.h"

#include <memory>

class ResourceMesh;

class ComponentMeshRenderer : public Component
{
public:
	ComponentMeshRenderer(const bool active, GameObject* owner, UID meshUID, UID textureUID);
	~ComponentMeshRenderer() override;

	void Init() override;
	void Update() override;

	void Draw();

	void SetMeshUID(UID& meshUID);
	void SetTextureUID(UID& textureUID);

	const UID& GetMeshUID() const;
	const UID& GetTextureUID() const;

private:
	void LoadMesh();
	bool IsMeshLoaded();

	std::shared_ptr<ResourceMesh> mesh = nullptr;
	
	UID meshUID = 0ULL;
	UID textureUID = 0ULL;
};

inline void ComponentMeshRenderer::SetTextureUID(UID& textureUID)
{
	this->textureUID = textureUID;
}

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