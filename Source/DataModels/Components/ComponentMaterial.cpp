#pragma warning (disable: 4804)
#pragma warning (disable: 4312)

#include "ComponentMaterial.h"

#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleEngineCamera.h"
#include "FileSystem/ModuleResources.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/Json.h"

#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceTexture.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowTextureInput.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowMaterialInput.h"
#include "DataModels/Program/Program.h"


#include <GL/glew.h>

ComponentMaterial::ComponentMaterial(bool active, GameObject* owner)
	: Component(ComponentType::MATERIAL, active, owner, true)
{
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::Update()
{
}

void ComponentMaterial::Draw()
{
	Program* program = App->program->GetProgram(ProgramType::MESHSHADER);
	if (program)
	{
		program->Activate();
		if (material)
		{
			std::shared_ptr<ResourceTexture> texture = App->resources->
				RequestResource<ResourceTexture>(material->GetDiffuseUID()).lock();

			program->BindUniformFloat3("material.diffuse_color", diffuseColor);

			if (texture)
			{
				if (!texture->IsLoaded())
				{
					texture->Load();
				}

				program->BindUniformInt("material.has_diffuse_map", 1);
				program->BindUniformInt("material.diffuse_map", texture->GetGlTexture());
				glActiveTexture(GL_TEXTURE0 + texture->GetGlTexture());
				glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
			}
			else
			{
				program->BindUniformInt("material.has_diffuse_map", 0);
			}

			texture = App->resources->RequestResource<ResourceTexture>(material->GetSpecularUID()).lock();
			if (texture)
			{
				if (!texture->IsLoaded())
				{
					texture->Load();
				}

				program->BindUniformInt("material.has_specular_map", 1);
				program->BindUniformInt("material.specular_map", texture->GetGlTexture());
				glActiveTexture(GL_TEXTURE0 + texture->GetGlTexture());
				glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
			}
			else
			{
				program->BindUniformInt("material.has_specular_map", 0);
			}

			texture = App->resources->RequestResource<ResourceTexture>(material->GetNormalUID()).lock();
			if (texture)
			{
				if (!texture->IsLoaded())
				{
					texture->Load();
				}

				glActiveTexture(GL_TEXTURE0 + texture->GetGlTexture());
				glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
				program->BindUniformInt("material.normal_map", texture->GetGlTexture());
				program->BindUniformFloat("material.normal_strength", normalStrength);

				program->BindUniformInt("material.has_normal_map", 1);
			}
			else
			{
				program->BindUniformInt("material.has_normal_map", 0);
			}

			program->BindUniformFloat3("material.specular_color", specularColor);
			program->BindUniformFloat("material.shininess", shininess);
			program->BindUniformFloat("material.shininess_alpha", hasShininessAlpha);

			float3 viewPos = App->engineCamera->GetPosition();
			program->BindUniformFloat3("viewPos", viewPos);
		}
		program->Deactivate();
	}
}

void ComponentMaterial::SaveOptions(Json& meta)
{
	// Do not delete these
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;

	UID uidMaterial = 0;
	std::string assetPath = "";

	if (material)
	{
		uidMaterial = material->GetUID();
		assetPath = material->GetAssetsPath();
	}
	meta["materialUID"] = (UID)uidMaterial;
	meta["assetPathMaterial"] = assetPath.c_str();

	meta["diffuseColor_X"] = (float)diffuseColor.x;
	meta["diffuseColor_Y"] = (float)diffuseColor.y;
	meta["diffuseColor_Z"] = (float)diffuseColor.z;

	meta["specularColor_X"] = (float)specularColor.x;
	meta["specularColor_Y"] = (float)specularColor.y;
	meta["specularColor_Z"] = (float)specularColor.z;

	meta["shininess"] = (float)shininess;
	meta["normalStrenght"] = (float)normalStrength;

	meta["hasShininessAlpha"] = (bool)hasShininessAlpha;
}

void ComponentMaterial::SaveUIDOfResourceToMeta(Json& meta, 
												const char* field, 
												const ResourceTexture* texturePtr)
{
	UID uidTexture = 0;

	if (texturePtr)
	{
		uidTexture = texturePtr->GetUID();
	}
	meta[field] = (UID)uidTexture;
}

void ComponentMaterial::LoadOptions(Json& meta)
{
	// Do not delete these
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];

	UID uidMaterial = meta["materialUID"];

	std::shared_ptr<ResourceMaterial> resourceMaterial = App->resources->RequestResource<ResourceMaterial>(uidMaterial).lock();

	if(resourceMaterial)
	{
		SetMaterial(resourceMaterial);
	}
	else 
	{
		std::string path = meta["assetPathMaterial"];
		bool resourceExists = path != "" && App->fileSystem->Exists(path.c_str());
		if (resourceExists) 
		{
			uidMaterial = App->resources->ImportResource(path);
			resourceMaterial = App->resources->RequestResource<ResourceMaterial>(uidMaterial).lock();
			SetMaterial(resourceMaterial);
		}
	}

	diffuseColor.x = (float)meta["diffuseColor_X"];
	diffuseColor.y = (float)meta["diffuseColor_Y"];
	diffuseColor.z = (float)meta["diffuseColor_Z"];

	specularColor.x = (float)meta["specularColor_X"];
	specularColor.y = (float)meta["specularColor_Y"];
	specularColor.z = (float)meta["specularColor_Z"];

	shininess = (float)meta["shininess"];
	normalStrength = (float)meta["normalStrenght"];

	hasShininessAlpha = (bool)meta["hasShininessAlpha"];
}

void ComponentMaterial::SetDiffuseUID(UID& diffuseUID)
{
	this->diffuseUID = diffuseUID;
}

void ComponentMaterial::SetNormalUID(UID& normalUID)
{
	this->normalUID = normalUID;
}

void ComponentMaterial::SetOcclusionUID(UID& occlusionUID)
{
	this->occlusionUID = occlusionUID;
}

void ComponentMaterial::SetSpecularUID(UID& specularUID)
{
	this->specularUID = specularUID;
}

void ComponentMaterial::SetMaterial(const std::shared_ptr<ResourceMaterial>& newMaterial)
{
	material = newMaterial;

	
	if (material)
	{
		material->Load();
		diffuseUID = material->GetDiffuseUID();
		normalUID = material->GetNormalUID();
		occlusionUID = material->GetOcclusionrUID();
		specularUID = material->GetSpecularUID();
	}
}

void ComponentMaterial::UnloadTextures()
{
	if(material)
	{
		std::shared_ptr<ResourceTexture> texture = App->resources->
										RequestResource<ResourceTexture>(material->GetDiffuseUID()).lock();
		if (texture)
		{
			texture->Unload();
		}

		texture = App->resources->RequestResource<ResourceTexture>(material->GetNormalUID()).lock();
		if (texture)
		{
			texture->Unload();
		}

		texture = App->resources->RequestResource<ResourceTexture>(material->GetOcclusionrUID()).lock();
		if (texture)
		{
			texture->Unload();
		}

		texture = App->resources->RequestResource<ResourceTexture>(material->GetSpecularUID()).lock();
		if (texture)
		{
			texture->Unload();
		}
	}
}

void ComponentMaterial::UnloadTexture(TextureType textureType)
{
	if (material)
	{
		std::shared_ptr<ResourceTexture> texture;
		switch (textureType)
		{
		case TextureType::DIFFUSE:
			texture = App->resources->RequestResource<ResourceTexture>(material->GetDiffuseUID()).lock();
			if (texture)
			{
				texture->Unload();
			}
			break;
		case TextureType::NORMAL:
			texture = App->resources->RequestResource<ResourceTexture>(material->GetNormalUID()).lock();
			if (texture)
			{
				texture->Unload();
			}
			break;
		case TextureType::OCCLUSION:
			texture = App->resources->RequestResource<ResourceTexture>(material->GetOcclusionrUID()).lock();
			if (texture)
			{
				texture->Unload();
			}
			break;
		case TextureType::SPECULAR:
			texture = App->resources->RequestResource<ResourceTexture>(material->GetSpecularUID()).lock();
			if (texture)
			{
				texture->Unload();
			}
			break;
		}
	}
}
