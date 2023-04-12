#pragma once

#include "Components/Component.h"
#include "Math/float4x4.h"

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

	void Init() override;

	void Update() override;

	void Draw() override;
	void DrawHighlight();

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void SetMesh(const std::shared_ptr<ResourceMesh>& newMesh);
	void SetMaterial(const std::shared_ptr<ResourceMaterial>& newMaterial);

	std::shared_ptr<ResourceMesh> GetMesh() const;
	std::shared_ptr<ResourceMaterial> GetMaterial() const;

	std::vector<float4x4> skinPalette;

private:
	bool IsMeshLoaded();
	bool IsMaterialLoaded();

	std::shared_ptr<ResourceMesh> mesh;
	std::shared_ptr<ResourceMaterial> material;

	WindowMeshInput* inputMesh;
};

inline std::shared_ptr<ResourceMesh> ComponentMeshRenderer::GetMesh() const
{
	return mesh;
}

inline std::shared_ptr<ResourceMaterial> ComponentMeshRenderer::GetMaterial() const
{
	return material;
}

inline bool ComponentMeshRenderer::IsMeshLoaded()
{
	return mesh != nullptr;
}