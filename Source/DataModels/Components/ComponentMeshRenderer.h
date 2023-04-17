#pragma once
#include "Component.h"
#include "Auxiliar/Generics/Drawable.h"

#include "Globals.h"

#include "Program/Program.h"

#include <memory>
#include "Math/float4.h"

class ResourceMaterial;
class ResourceMesh;
class Json;
class WindowMeshInput;

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
	void LoadOptions(Json& meta) override;

	void SetMesh(const std::shared_ptr<ResourceMesh>& newMesh);
	void SetMaterial(const std::shared_ptr<ResourceMaterial>& newMaterial);

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
	bool IsMeshLoaded() const;
	bool IsMaterialLoaded() const;

	//declared "mutable" so Draw can be const
	//as said in Draw, this should be modified in a separate class, so the idea is for this change to be temporal
	mutable std::shared_ptr<ResourceMesh> mesh;
	mutable std::shared_ptr<ResourceMaterial> material;

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
inline bool ComponentMeshRenderer::IsMeshLoaded() const
{
	return mesh != nullptr;
}
inline bool ComponentMeshRenderer::IsMaterialLoaded() const
{
	return material != nullptr;
}