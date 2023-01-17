#pragma once
#include "Components/Component.h"

#include "Math/float3.h"

#include "FileSystem/UniqueID.h"

#include <memory>
#include "ImGuiFileDialog/ImGuiFileDialog.h"
#include "FileSystem/Importers/TextureImporter.h"

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

	std::weak_ptr<ResourceTexture> textureDiffuse;
	std::weak_ptr<ResourceTexture> textureNormal;
	std::weak_ptr<ResourceTexture> textureOcclusion;
	std::weak_ptr<ResourceTexture> textureSpecular;

	float3 diffuseColor = float3(1.0, 1.0, 0.0);
	float3 specularColor = float3(0.5, 0.5, 0.5);
	float shininess = 512.f;
	float normalStrength = 1.0;

	bool hasShininessAlpha = true;
	//bool hasNormal = false;

	const char* dialogName;
	const char* title;
	const char* filters;
	const char* startPath;
	std::shared_ptr<TextureImporter> textureImporter;

	ImGuiFileDialog fileDialogImporter;

	//Auxiliar UIDs
	UID diffuseUID = 0;
	UID normalUID = 0;
	UID occlusionUID = 0;
	UID specularUID = 0;
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