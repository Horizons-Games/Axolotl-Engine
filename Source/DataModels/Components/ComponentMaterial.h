#pragma once
#include "Components/Component.h"

#include "Math/float3.h"

#include "FileSystem/UniqueID.h"

#include <memory>

#define COMPONENT_MATERIAL "Material"


class ResourceMaterial;
class ResourceTexture;
class Json;

enum class TextureType { DIFFUSE, NORMAL, OCCLUSION, SPECULAR };

class ComponentMaterial : public Component
{
public:
	ComponentMaterial(bool active, GameObject* owner);
	~ComponentMaterial() override;

	void Update() override;

	void Draw() override;
	void Display() override;

	void SaveOptions(Json& meta) override;
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
	void SetHasDisffuse(bool hasDiffuse);
	void SetHasNormal(bool hasNormal);
	void SetHasOcclusion(bool hasOcclusion);
	void SetHasSpecular(bool hasSpecular);
	void SetHasShininessAlpha(bool hasShininessAlpha);

	std::string GetTypeName() override;


	std::weak_ptr<ResourceMaterial> GetMaterial() const;
	const UID& GetDiffuseUID() const;
	const UID& GetNormalUID() const;
	const UID& GetOcclusionUID() const;
	const UID& GetSpecularUID() const;
	const float3& GetDiffuseColor() const;
	const float3 GetSpecularColor() const;
	const float GetShininess() const;
	const float GetNormalStrenght() const;
	const bool HasDisffuse() const;
	const bool HasNormal() const;
	const bool HasOcclusion() const;
	const bool HasSpecular() const;
	const bool HasShininessAlpha() const;

private:

	void LoadTexture();
	void LoadTexture(TextureType textureType);

	std::weak_ptr<ResourceMaterial> material;

	std::weak_ptr<ResourceTexture> textureDiffuse;
	std::weak_ptr<ResourceTexture> textureNormal;
	std::weak_ptr<ResourceTexture> textureOcclusion;
	std::weak_ptr<ResourceTexture> textureSpecular;

	// I don't see utility
	//std::vector<unsigned> textureWidths;
	//std::vector<unsigned> textureHeights;

	UID diffuseUID = 0;
	UID normalUID = 0;
	UID occlusionUID = 0;
	UID specularUID = 0;
	float3 diffuseColor = float3(1.0, 1.0, 0.0);
	float3 specularColor = float3(0.5, 0.5, 0.5);
	float shininess = 512.f;
	float normalStrength = 1.0;

	//TODO change UID or leave this here meanwhile this is for MaterialImporter and Component Material charge
	bool hasDiffuse = false;
	bool hasNormal = false;
	bool hasOcclusion = false;
	bool hasSpecular = false;
	bool hasShininessAlpha = false;

};

inline std::string ComponentMaterial::GetTypeName()
{
	return COMPONENT_MATERIAL;
}

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

inline void ComponentMaterial::SetHasDisffuse(bool hasDiffuse)
{
	this->hasDiffuse = hasDiffuse;
}

inline void ComponentMaterial::SetHasNormal(bool hasNormal)
{
	this->hasNormal = hasNormal;
}

inline void ComponentMaterial::SetHasOcclusion(bool hasOcclusion)
{
	this->hasOcclusion = hasOcclusion;
}

inline void ComponentMaterial::SetHasSpecular(bool hasSpecular)
{
	this->hasSpecular = hasSpecular;
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

inline const bool ComponentMaterial::HasDisffuse() const {
	return hasDiffuse;
}

inline const bool ComponentMaterial::HasNormal() const {
	return hasNormal;
}

inline const bool ComponentMaterial::HasOcclusion() const {
	return hasOcclusion;
}

inline const bool ComponentMaterial::HasSpecular() const {
	return hasSpecular;
}

inline const bool ComponentMaterial::HasShininessAlpha() const {
	return hasShininessAlpha;
}