#pragma once

#include "Components/Component.h"

#include <memory>

#define COMPONENT_MESHRENDERED "MeshRendered"

class ResourceMaterial;
class ResourceMesh;
class Json;
class WindowMeshInput;

class ComponentMeshRenderer : public Component
{
public:
	ComponentMeshRenderer(const bool active, GameObject* owner);
	ComponentMeshRenderer(const ComponentMeshRenderer& componentMeshRenderer);
	~ComponentMeshRenderer() override;

	void Update() override;

	void Draw() override;
	void DrawHighlight();

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void SetMesh(const std::shared_ptr<ResourceMesh>& newMesh);

	std::shared_ptr<ResourceMesh> GetMesh() const;

private:
	bool IsMeshLoaded();

	std::shared_ptr<ResourceMesh> mesh;

	WindowMeshInput* inputMesh;
};

inline std::shared_ptr<ResourceMesh> ComponentMeshRenderer::GetMesh() const
{
	return mesh;
}

inline bool ComponentMeshRenderer::IsMeshLoaded()
{
	return mesh != nullptr;
}