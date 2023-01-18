#pragma once
#include "Components/Component.h"

#include "Math/float3.h"

#include "FileSystem/UniqueID.h"
#include "Globals.h"

#include <memory>

#define COMPONENT_MATERIAL "Material"

class WindowTextureInput;
class ResourceMaterial;
class ResourceTexture;
class Json;

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(bool active, GameObject* owner);
	~ComponentMaterial() override;

	void Update() override;

	void Draw() override;
	void Display() override;

	void SaveOptions(Json& meta) override;
	void SaveUIDOfResourceToMeta(Json& meta, const char* field, const std::weak_ptr<ResourceTexture>& texturePtr);
	void LoadOptions(Json& meta) override;

	void SetMaterial(const std::weak_ptr<ResourceMaterial>& newMaterial);
	void SetDiffuseUID(UID& diffuseUID);
	void SetNormalUID(UID& normalUID);
	void SetOcclusionUID(UID& occlusionUID);
	void SetSpecularUID(UID& specularUID);
	void SetDiffuseColor(float3& diffuseColor);
	void SetSpecularColor(float3& specularColor);
	void SetShininess(float shininess);
	void SetNormalStrenght(float normalStrength);
	void SetHasShininessAlpha(bool hasShininessAlpha);

	std::weak_ptr<ResourceMaterial> GetMaterial() const;
	const UID& GetDiffuseUID() const;
	const UID& GetNormalUID() const;
	const UID& GetOcclusionUID() const;
	const UID& GetSpecularUID() const;
	const float3& GetDiffuseColor() const;
	const float3 GetSpecularColor() const;
	const float GetShininess() const;
	const float GetNormalStrenght() const;
	const bool HasShininessAlpha() const;

private:

	void LoadTexture();
	void LoadTexture(TextureType textureType);

	std::weak_ptr<ResourceMaterial> material;

	float3 diffuseColor = float3(1.0, 1.0, 0.0);
	float3 specularColor = float3(0.5, 0.5, 0.5);
	float shininess = 512.f;
	float normalStrength = 1.0f;

	bool hasShininessAlpha = false;

	std::unique_ptr<WindowTextureInput> inputTextureDiffuse;
	std::unique_ptr<WindowTextureInput> inputTextureNormal;
	std::unique_ptr<WindowTextureInput> inputTextureSpecular;

	//This has to be erased
	std::weak_ptr<ResourceTexture> textureDiffuse;
	std::weak_ptr<ResourceTexture> textureNormal;
	std::weak_ptr<ResourceTexture> textureOcclusion;
	std::weak_ptr<ResourceTexture> textureSpecular;

	//Auxiliar UIDs
	UID diffuseUID = 0;
	UID normalUID = 0;
	UID occlusionUID = 0;
	UID specularUID = 0;
	//All this
};

inline void ComponentMaterial::SetDiffuseColor(float3& diffuseColor)
{
	this->diffuseColor = diffuseColor;
}

inline void ComponentMaterial::SetSpecularColor(float3& specularColor)
{
	this->specularColor = specularColor;
}

inline void ComponentMaterial::SetShininess(float shininess)
{
	this->shininess = shininess;
}

inline void ComponentMaterial::SetNormalStrenght(float normalStrength)
{
	this->normalStrength = normalStrength;
}

inline void ComponentMaterial::SetHasShininessAlpha(bool hasShininessAlpha)
{
	this->hasShininessAlpha = hasShininessAlpha;
}

inline std::weak_ptr<ResourceMaterial> ComponentMaterial::GetMaterial() const
{
	return material;
}

inline const UID& ComponentMaterial::GetDiffuseUID() const {
	return diffuseUID;
}

inline const UID& ComponentMaterial::GetNormalUID() const {
	return normalUID;
}

inline const UID& ComponentMaterial::GetOcclusionUID() const {
	return occlusionUID;
}

inline const UID& ComponentMaterial::GetSpecularUID() const {
	return specularUID;
}

inline const float3& ComponentMaterial::GetDiffuseColor() const {
	return diffuseColor;
}

inline const float3 ComponentMaterial::GetSpecularColor() const {
	return specularColor;
}

inline const float ComponentMaterial::GetShininess() const {
	return shininess;
}

inline const float ComponentMaterial::GetNormalStrenght() const {
	return normalStrength;
}

inline const bool ComponentMaterial::HasShininessAlpha() const {
	return hasShininessAlpha;
}