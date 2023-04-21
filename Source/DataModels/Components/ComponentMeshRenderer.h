#pragma once

#include "Globals.h"

#include "Components/Component.h"
#include "ModuleProgram.h"

#include "FileSystem/UniqueID.h"

#include "Math/float3.h"
#include "Math/float4.h"

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
	void DrawMeshes(Program* program);
	void DrawMaterial(Program* program);
	void DrawHighlight();

	void SaveOptions(Json& meta) override;
	/*void SaveUIDOfResourceToMeta
		(Json& meta, const char* field, const ResourceTexture* texturePtr);*/
	void LoadOptions(Json& meta) override;

	void SetMesh(const std::shared_ptr<ResourceMesh>& newMesh);
	void SetMaterial(const std::shared_ptr<ResourceMaterial>& newMaterial);

	// Common attributes (setters)
	void SetDiffuseColor(float4& diffuseColor);
	void SetSmoothness(float smoothness);
	void SetNormalStrenght(float normalStrength);

	// Default shader attributes (setters)
	void SetMetalness(float metalness);

	// Specular shader attributes (setters)
	void SetSpecularColor(float3& specularColor);
	
	std::shared_ptr<ResourceMesh> GetMesh() const;
	std::shared_ptr<ResourceMaterial> GetMaterial() const;
	const unsigned int GetShaderType() const;

	// Common attributes (getters)
	const float4& GetDiffuseColor() const;
	const float GetSmoothness() const;
	const float GetNormalStrenght() const;

	// Default shader attributes (getters)
	const float GetMetalness() const;

	// Specular shader attributes (getters)
	const float3& GetSpecularColor() const;

	void UnloadTextures();
	void UnloadTexture(TextureType textureType);

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

inline std::shared_ptr<ResourceMaterial> 
							ComponentMeshRenderer::GetMaterial() const
{
	return material;
}

inline bool ComponentMeshRenderer::IsMeshLoaded()
{
	return mesh != nullptr;
}
inline bool ComponentMeshRenderer::IsMaterialLoaded()
{
	return material != nullptr;
}