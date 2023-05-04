#pragma once
#include "Component.h"
#include "Auxiliar/Generics/Drawable.h"

#include "Globals.h"

#include "Components/Component.h"

#include "FileSystem/UniqueID.h"
#include "Batch/GeometryBatch.h"


#include <memory>

#include "FileSystem/UniqueID.h"

#include "Math/float3.h"
#include "Math/float4.h"

class ResourceMesh;
class ResourceMaterial;
class ResourceTexture;
class Json;
class WindowMeshInput;
class WindowMaterialInput;
class WindowTextureInput;

class ComponentMeshRenderer : public Component, public Drawable
{
public:
	ComponentMeshRenderer(const bool active, GameObject* owner);
	ComponentMeshRenderer(const ComponentMeshRenderer& componentMeshRenderer);
	~ComponentMeshRenderer() override;

	void Draw() const override;
	void DrawMeshes(Program* program) const;
	void DrawMaterial(Program* program) const;
	void DrawHighlight() const;

	void SaveOptions(Json& meta) override;
	/*void SaveUIDOfResourceToMeta
		(Json& meta, const char* field, const ResourceTexture* texturePtr);*/
	void LoadOptions(Json& meta) override;

	void SetMesh(const std::shared_ptr<ResourceMesh>& newMesh);
	void SetMaterial(const std::shared_ptr<ResourceMaterial>& newMaterial);

	// Common attributes (setters)
	void SetDiffuseColor(float4& diffuseColor);
	void SetDiffuse(const std::shared_ptr<ResourceTexture>& diffuse);
	void SetNormal(const std::shared_ptr<ResourceTexture>& normal);
	void SetMetallic(const std::shared_ptr<ResourceTexture>& metallic);
	void SetSpecular(const std::shared_ptr<ResourceTexture>& specular);
	void SetShaderType(unsigned int shaderType);
	void SetSmoothness(float smoothness);
	void SetNormalStrength(float normalStrength);

	// Default shader attributes (setters)
	void SetMetalness(float metalness);

	// Specular shader attributes (setters)
	void SetSpecularColor(float3& specularColor);

	void SetTransparent(bool isTransparent);

	void RemoveFromBatch();

	std::vector<ComponentMeshRenderer*> ChangeOfBatch();

	std::shared_ptr<ResourceMesh> GetMesh() const;
	std::shared_ptr<ResourceMaterial> GetMaterial() const;

	GeometryBatch* GetBatch() const;
	void SetBatch(GeometryBatch* geometryBatch);

	// Common attributes (getters)
	const unsigned int& GetShaderType() const;
	const float4& GetDiffuseColor() const;
	const float GetSmoothness() const;
	const float GetNormalStrenght() const;

	// Default shader attributes (getters)
	const float GetMetalness() const;

	// Specular shader attributes (getters)
	const float3& GetSpecularColor() const;

	const bool IsTransparent() const;

	const std::shared_ptr<ResourceTexture>& GetDiffuse() const;

	const std::shared_ptr<ResourceTexture>& GetNormal() const;

	const std::shared_ptr<ResourceTexture>& GetOcclusion() const;

	const std::shared_ptr<ResourceTexture>& GetMetallic() const;

	const std::shared_ptr<ResourceTexture>& GetSpecular() const;

	void UnloadTextures();
	void UnloadTexture(TextureType textureType);

private:
	bool IsMeshLoaded() const;
	bool IsMaterialLoaded() const;

	mutable std::shared_ptr<ResourceMesh> mesh;
	mutable std::shared_ptr<ResourceMaterial> material;

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

inline bool ComponentMeshRenderer::IsMeshLoaded() const
{
	return mesh != nullptr;
}

inline bool ComponentMeshRenderer::IsMaterialLoaded() const
{
	return material != nullptr;
}