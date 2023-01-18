#pragma once

#include "Components/Component.h"
#include "FileSystem/UniqueID.h"

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

	void Update() override;

	void Draw() override;
	void Display() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void SetMesh(const std::weak_ptr<ResourceMesh>& newMesh);

	std::weak_ptr<ResourceMesh> GetMesh() const;

private:
	bool IsMeshLoaded();

	std::weak_ptr<ResourceMesh> mesh;

	std::unique_ptr<WindowMeshInput> inputMesh;
};

inline std::weak_ptr<ResourceMesh> ComponentMeshRenderer::GetMesh() const
{
	return mesh;
}

inline bool ComponentMeshRenderer::IsMeshLoaded()
{
	return !mesh.expired();
}