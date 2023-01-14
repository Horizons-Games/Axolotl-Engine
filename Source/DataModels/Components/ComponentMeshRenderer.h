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
	ComponentMeshRenderer(const bool active, GameObject* owner, UID meshUID, UID textureUID); // This Constructor will disappear
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

	std::shared_ptr<ResourceMesh> mesh = nullptr;
	std::shared_ptr<ResourceTexture> texture = nullptr;
	
	UID meshUID = 0LL;
	UID textureUID = 0LL;
};

inline const UID& ComponentMeshRenderer::GetMeshUID() const
{
	return meshUID;
}

inline const UID& ComponentMeshRenderer::GetTextureUID() const
{
	return textureUID;
}