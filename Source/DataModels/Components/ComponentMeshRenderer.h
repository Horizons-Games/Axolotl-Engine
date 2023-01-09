#pragma once

#include <memory>

#include "Components/Component.h"

class ResourceMesh;
class ResourceTexture;

class ComponentMeshRenderer : public Component
{
public:
	ComponentMeshRenderer(const bool active, GameObject* owner);
	ComponentMeshRenderer(const bool active, GameObject* owner, 
	UID meshUID, UID textureUID, const std::string& meshPath, const std::string& texturePath);
	~ComponentMeshRenderer() override;

	void Update() override;

	void Draw();

private:
	std::unique_ptr<ResourceMesh> mesh = nullptr;
	std::unique_ptr<ResourceTexture> texture = nullptr;
};