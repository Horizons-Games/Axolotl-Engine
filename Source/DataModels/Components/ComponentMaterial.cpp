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
	unsigned int program = App->program->GetProgram();

	GLint programInUse;
	glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

	if (program != programInUse)
	{
		glUseProgram(program);
	}

	if(material) 
	{
		glUniform3f(3, diffuseColor.x, diffuseColor.y, diffuseColor.z); //diffuse_color
		std::shared_ptr<ResourceTexture> texture = App->resources->
										RequestResource<ResourceTexture>(material->GetDiffuseUID()).lock();
		if (texture)
		{
			if (!texture->IsLoaded())
			{
				texture->Load();
			}

			glUniform1i(7, 1); //has_diffuse_map
			
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
		}
		else
		{
			glUniform1i(7, 0); //has_diffuse_map
		}

		glUniform3f(4, specularColor.x, specularColor.y, specularColor.z); //specular_color
		texture = App->resources->RequestResource<ResourceTexture>(material->GetSpecularUID()).lock();
		if (texture)
		{
			if (!texture->IsLoaded())
			{
				texture->Load();
			}

			glUniform1i(8, 1); //has_specular_map
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
		}
		else
		{
			glUniform1i(8, 0); //has_specular_map
		}

		texture = App->resources->RequestResource<ResourceTexture>(material->GetNormalUID()).lock();
		if (texture)
		{
			if (!texture->IsLoaded())
			{
				texture->Load();
			}

			glActiveTexture(GL_TEXTURE7);
			glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
			glUniform1f(6, normalStrength); //normal_strength
			glUniform1i(11, 1); //has_normal_map
		}
		else
		{
			glUniform1i(11, 0); //has_normal_map
		}

		glUniform1f(5, shininess); //shininess
		glUniform1f(9, hasShininessAlpha); //shininess_alpha

		float3 viewPos = App->engineCamera->GetPosition();
		glUniform3f(glGetUniformLocation(program, "viewPos"), viewPos.x, viewPos.y, viewPos.z);
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

	std::shared_ptr<ResourceMaterial> resourceMaterial = 
		App->resources->RequestResource<ResourceMaterial>(uidMaterial).lock();

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
