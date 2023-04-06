#pragma once

#include "Components/Component.h"

#include "Math/float3.h"

#include "FileSystem/UniqueID.h"
#include "Globals.h"

#include <memory>

#define COMPONENT_MESHRENDERED "MeshRendered"

class ResourceMesh;
class ResourceMaterial;
class ResourceTexture;
class Json;
class WindowMeshInput;
class WindowMaterialInput;
class WindowTextureInput;

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
	void SaveUIDOfResourceToMeta(Json& meta, const char* field, const ResourceTexture* texturePtr);
	void LoadOptions(Json& meta) override;

	void SetMesh(const std::shared_ptr<ResourceMesh>& newMesh);
	void SetMaterial(const std::shared_ptr<ResourceMaterial>& newMaterial);
	void SetDiffuseColor(float3& diffuseColor);
	void SetSpecularColor(float3& specularColor);
	void SetShininess(float shininess);
	void SetNormalStrenght(float normalStrength);
	void SetSmoothness(float smoothness);
	void SetMetalness(float metalness);
	void SetHasShininessAlpha(bool hasShininessAlpha);
	void SetMetallicAlpha(bool metallicAlpha);

	std::shared_ptr<ResourceMesh> GetMesh() const;
	std::shared_ptr<ResourceMaterial> GetMaterial() const;
	const float3& GetDiffuseColor() const;
	const float3& GetSpecularColor() const;
	const float GetShininess() const;
	const float GetNormalStrenght() const;
	const float GetSmoothness() const;
	const float GetMetalness() const;
	const bool HasShininessAlpha() const;
	const bool HasMetallicAlpha() const;
private:
	bool IsMeshLoaded();
	void UnloadTextures();
	void UnloadTexture(TextureType textureType);

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