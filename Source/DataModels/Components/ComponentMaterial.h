#pragma once
#pragma warning (disable: 26495)

#include "Components/Component.h"

#include "Math/float3.h"

#include "FileSystem/UniqueID.h"
#include "Globals.h"

class WindowTextureInput;
class WindowMaterialInput;
class ResourceMaterial;
class ResourceTexture;
class Json;

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(bool active, GameObject* owner);
	ComponentMaterial(const ComponentMaterial& componentMaterial);
	~ComponentMaterial() override;

	void Update() override;

	void Draw() override;

	void SaveOptions(Json& meta) override;
	void SaveUIDOfResourceToMeta(Json& meta, const char* field, const ResourceTexture* texturePtr);
	void LoadOptions(Json& meta) override;

	void SetMaterial(const std::shared_ptr<ResourceMaterial>& newMaterial);
	void SetDiffuseColor(float3& diffuseColor);
	//void SetSpecularColor(float3& specularColor);
	//void SetShininess(float shininess);
	void SetNormalStrenght(float normalStrength);
	void SetSmoothness(float smoothness);
	void SetMetalness(float metalness);
	//void SetHasShininessAlpha(bool hasShininessAlpha);
	void SetMetallicAlpha(bool metallicAlpha);
	void SetTransparent(bool isTransparent);

	std::shared_ptr<ResourceMaterial> GetMaterial() const;
	const float4& GetDiffuseColor() const;
	const float3& GetSpecularColor() const;
	//const float GetShininess() const;
	const float GetNormalStrenght() const;
	const float GetSmoothness() const;
	const float GetMetalness() const;
	//const bool HasShininessAlpha() const;
	const bool HasMetallicAlpha() const;
	const bool GetTransparent() const;

private:

	void UnloadTextures();
	void UnloadTexture(TextureType textureType);

	std::shared_ptr<ResourceMaterial> material;
	bool isTransparent = false;

	friend class WindowComponentMaterial;
};

inline std::shared_ptr<ResourceMaterial> ComponentMaterial::GetMaterial() const
{
	return material;
}

inline const bool ComponentMaterial::GetTransparent() const
{
	return isTransparent;
}

inline void ComponentMaterial::SetTransparent(bool isTransparent)
{
	this->isTransparent = isTransparent;
}