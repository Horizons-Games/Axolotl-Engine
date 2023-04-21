#pragma once

#include "Globals.h"

#include "Components/Component.h"

#include "Math/float4x4.h"
#include "Math/float4.h"

#include "Program/Program.h"

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

	void InitBones();

	void Update() override;

	void Draw() override;
	void DrawMeshes(Program* program);
	void DrawMaterial(Program* program);
	void DrawHighlight();

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void SetMesh(const std::shared_ptr<ResourceMesh>& newMesh);
	void SetMaterial(const std::shared_ptr<ResourceMaterial>& newMaterial);
	void SetBones(const std::vector<GameObject*>& bones);

	std::shared_ptr<ResourceMesh> GetMesh() const;
	std::shared_ptr<ResourceMaterial> GetMaterial() const;

	void UnloadTextures();
	void UnloadTexture(TextureType textureType);

	//material
	void SetDiffuseColor(float4& diffuseColor);
	//void SetSpecularColor(float3& specularColor);
	//void SetShininess(float shininess);
	void SetNormalStrenght(float normalStrength);
	void SetSmoothness(float smoothness);
	void SetMetalness(float metalness);
	//void SetHasShininessAlpha(bool hasShininessAlpha);
	void SetMetallicAlpha(bool metallicAlpha);

	const float4& GetDiffuseColor() const;
	//const float3& GetSpecularColor() const;
	//const float GetShininess() const;
	const float GetNormalStrenght() const;
	const float GetSmoothness() const;
	const float GetMetalness() const;
	//const bool HasShininessAlpha() const;
	const bool HasMetallicAlpha() const;

private:
	bool IsMeshLoaded();
	bool IsMaterialLoaded();

	std::shared_ptr<ResourceMesh> mesh;
	std::shared_ptr<ResourceMaterial> material;

	std::vector<GameObject*> bones;
	std::vector<float4x4> skinPalette;

	unsigned ssboPalette;

	WindowMeshInput* inputMesh;
};

inline void ComponentMeshRenderer::SetBones(const std::vector<GameObject*>& bones)
{
	this->bones = bones;
}

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
inline bool ComponentMeshRenderer::IsMaterialLoaded()
{
	return material != nullptr;
}