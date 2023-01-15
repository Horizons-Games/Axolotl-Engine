#pragma once

#include "Components/Component.h"
#include "FileSystem/UniqueID.h"

#include <memory>

#define COMPONENT_MESHRENDERED "MeshRendered"

class ResourceMaterial;
class ResourceMesh;
class Json;

class ComponentMeshRenderer : public Component
{
public:
	ComponentMeshRenderer(const bool active, GameObject* owner);
	~ComponentMeshRenderer() override;

	void Update() override;

	void Draw() override;
	void Display() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void SetMesh(const std::weak_ptr<ResourceMesh>& newMesh);

	std::string GetTypeName() override;

	std::weak_ptr<ResourceMesh> GetMesh() const;

private:
	bool IsMeshLoaded();

	std::weak_ptr<ResourceMesh> mesh;
};

inline std::string ComponentMeshRenderer::GetTypeName()
{
	return COMPONENT_MESHRENDERED;
}

inline std::weak_ptr<ResourceMesh> ComponentMeshRenderer::GetMesh() const
{
	return mesh;
}

inline bool ComponentMeshRenderer::IsMeshLoaded()
{
	return !mesh.expired();
}