#pragma once

#include "Components/Component.h"
#include "FileSystem/UniqueID.h"

#define COMPONENT_MESHRENDERED "MeshRendered"

class ResourceMaterial;
class ResourceMesh;
class Json;
class WindowMeshInput;

class ComponentMeshRenderer : public Component
{
public:
	ComponentMeshRenderer(const bool active, GameObject* owner);
	~ComponentMeshRenderer() override;

	void Update() override;

	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void SetMesh(ResourceMesh* newMesh);

	ResourceMesh* GetMesh() const;

private:
	bool IsMeshLoaded();

	ResourceMesh* mesh;

	WindowMeshInput* inputMesh;
};

inline ResourceMesh* ComponentMeshRenderer::GetMesh() const
{
	return mesh;
}

inline bool ComponentMeshRenderer::IsMeshLoaded()
{
	return mesh != nullptr;
}