#pragma once

#include "Globals.h"

#include "Components/Component.h"

#include "FileSystem/UniqueID.h"
#include "Batch/GeometryBatch.h"


#include <memory>

#define COMPONENT_MESHRENDERED "MeshRendered"

class ResourceMaterial;
class ResourceMesh;
class Json;
class WindowMeshInput;
class WindowMaterialInput;

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
	void SetMaterial(const std::shared_ptr<ResourceMaterial>& newMaterial);

	std::shared_ptr<ResourceMesh> GetMesh() const;
	std::shared_ptr<ResourceMaterial> GetMaterial() const;

	GeometryBatch* GetBatch() const;
	void SetBatch(GeometryBatch* geometryBatch);

	void UnloadTextures();
	void UnloadTexture(TextureType textureType);

	//material
	void SetDiffuseColor(float3& diffuseColor);
	//void SetSpecularColor(float3& specularColor);
	//void SetShininess(float shininess);
	void SetNormalStrenght(float normalStrength);
	void SetSmoothness(float smoothness);
	void SetMetalness(float metalness);
	//void SetHasShininessAlpha(bool hasShininessAlpha);
	void SetMetallicAlpha(bool metallicAlpha);

	const float3& GetDiffuseColor() const;
	//const float3& GetSpecularColor() const;
	//const float GetShininess() const;
	const float GetNormalStrenght() const;
	const float GetSmoothness() const;
	const float GetMetalness() const;
	//const bool HasShininessAlpha() const;
	const bool HasMetallicAlpha() const;

	bool materialToUpdate = false;

private:
	bool IsMeshLoaded();
	bool IsMaterialLoaded();

	std::shared_ptr<ResourceMesh> mesh;
	std::shared_ptr<ResourceMaterial> material;

	GeometryBatch* batch;
	WindowMeshInput* inputMesh;
	WindowMaterialInput* inputMaterial;

};

inline std::shared_ptr<ResourceMesh> ComponentMeshRenderer::GetMesh() const
{
	return mesh;
}

inline std::shared_ptr<ResourceMaterial> ComponentMeshRenderer::GetMaterial() const
{
	return material;
}

inline GeometryBatch* ComponentMeshRenderer::GetBatch() const
{
	return batch;
}

inline void ComponentMeshRenderer::SetBatch(GeometryBatch* geometryBatch)
{
	batch = geometryBatch;
}

inline bool ComponentMeshRenderer::IsMeshLoaded()
{
	return mesh != nullptr;
}
inline bool ComponentMeshRenderer::IsMaterialLoaded()
{
	return material != nullptr;
}