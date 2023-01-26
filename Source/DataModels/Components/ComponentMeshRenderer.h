#pragma once

#include "Components/Component.h"
#include "FileSystem/UniqueID.h"
#include "Batch/GeometryBatch.h"

#include <memory>

#define COMPONENT_MESHRENDERED "MeshRendered"

class ResourceMaterial;
class ResourceMesh;
class Json;
class WindowMeshInput;

class ComponentMeshRenderer : public Component
{
public:
	ComponentMeshRenderer(const bool active, const std::shared_ptr<GameObject>& owner);
	~ComponentMeshRenderer() override;

	void Update() override;

	void Draw() override;
	void Display() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void SetMesh(UID meshUID);

	UID GetMeshUID() const;
	std::weak_ptr<ResourceMesh> GetMesh() const;

private:
	bool IsMeshLoaded();

	UID meshUID;
	//std::weak_ptr<ResourceMesh> mesh;
	std::unique_ptr<GeometryBatch> batch;
	std::unique_ptr<WindowMeshInput> inputMesh;
};

inline std::weak_ptr<ResourceMesh> ComponentMeshRenderer::GetMesh() const
{
	return batch->GetMesh(meshUID);
}

inline UID ComponentMeshRenderer::GetMeshUID() const
{
	return meshUID;
}

inline bool ComponentMeshRenderer::IsMeshLoaded()
{
	return !batch->GetMesh(meshUID).expired();
}