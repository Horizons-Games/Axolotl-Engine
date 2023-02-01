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

ComponentMaterial::ComponentMaterial(bool active, const std::shared_ptr<GameObject>& owner)
	: Component(ComponentType::MATERIAL, active, owner, true)
{
}

ComponentMaterial::~ComponentMaterial()
{
	UnloadTextures();
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

	std::shared_ptr<ResourceMaterial> materialAsShared = material.lock();

	if(materialAsShared) 
	{
		std::shared_ptr<ResourceTexture> texture = App->resources->
										RequestResource<ResourceTexture>(materialAsShared->GetDiffuseUID()).lock();

		glUniform3f(glGetUniformLocation(program, "material.diffuse_color"), 
					diffuseColor.x, diffuseColor.y, diffuseColor.z);
		if (texture)
		{
			if (!texture->IsLoaded())
			{
				texture->Load();
			}

			glUniform1i(glGetUniformLocation(program, "material.has_diffuse_map"), 1);
			glUniform1i(glGetUniformLocation(program, "material.diffuse_map"), texture->GetGlTexture());
			glActiveTexture(GL_TEXTURE0 + texture->GetGlTexture());
			glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
		}
		else
		{
			glUniform1i(glGetUniformLocation(program, "material.has_diffuse_map"), 0);
		}

		texture = App->resources->RequestResource<ResourceTexture>(materialAsShared->GetSpecularUID()).lock();
		if (texture)
		{
			if (!texture->IsLoaded())
			{
				texture->Load();
			}

			glUniform1i(glGetUniformLocation(program, "material.has_specular_map"), 1);
			glUniform1i(glGetUniformLocation(program, "material.specular_map"), texture->GetGlTexture());
			glActiveTexture(GL_TEXTURE0 + texture->GetGlTexture());
			glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
		}
		else
		{
			glUniform1i(glGetUniformLocation(program, "material.has_specular_map"), 0);
		}

		texture = App->resources->RequestResource<ResourceTexture>(materialAsShared->GetNormalUID()).lock();
		if (texture)
		{
			if (!texture->IsLoaded())
			{
				texture->Load();
			}

			glActiveTexture(GL_TEXTURE0 + texture->GetGlTexture());
			glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
			glUniform1i(glGetUniformLocation(program, "material.normal_map"), texture->GetGlTexture());
			glUniform1f(glGetUniformLocation(program, "material.normal_strength"), normalStrength);

			glUniform1i(glGetUniformLocation(program, "material.has_normal_map"), 1);
		}
		else
		{
			glUniform1i(glGetUniformLocation(program, "material.has_normal_map"), 0);
		}

		glUniform3f(glGetUniformLocation(program, "material.specular_color"), 
					specularColor.x, specularColor.y, specularColor.z);
		glUniform1f(glGetUniformLocation(program, "material.shininess"), shininess);
		glUniform1f(glGetUniformLocation(program, "material.shininess_alpha"), hasShininessAlpha);

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

	std::shared_ptr<ResourceMaterial> materialAsShared = material.lock();
	UID uidMaterial = 0;
	std::string assetPath = "";

	if (materialAsShared)
	{
		uidMaterial = materialAsShared->GetUID();
		assetPath = materialAsShared->GetAssetsPath();
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
												const std::weak_ptr<ResourceTexture>& texturePtr)
{
	std::shared_ptr<ResourceTexture> textureAsShared = texturePtr.lock();
	UID uidTexture = 0;

	if (textureAsShared)
	{
		uidTexture = textureAsShared->GetUID();
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

void ComponentMaterial::SetMaterial(const std::weak_ptr<ResourceMaterial>& newMaterial)
{
	material = newMaterial;
	std::shared_ptr<ResourceMaterial> materialAsShared = material.lock();

	
	if (materialAsShared)
	{
		materialAsShared->Load();
		diffuseUID = materialAsShared->GetDiffuseUID();
		normalUID = materialAsShared->GetNormalUID();
		occlusionUID = materialAsShared->GetOcclusionrUID();
		specularUID = materialAsShared->GetSpecularUID();
	}
}

void ComponentMaterial::UnloadTextures()
{
	std::shared_ptr<ResourceMaterial> materialAsShared = material.lock();

	if(materialAsShared)
	{
		std::shared_ptr<ResourceTexture> texture = App->resources->
										RequestResource<ResourceTexture>(materialAsShared->GetDiffuseUID()).lock();
		if (texture)
		{
			texture->Unload();
		}

		texture = App->resources->RequestResource<ResourceTexture>(materialAsShared->GetNormalUID()).lock();
		if (texture)
		{
			texture->Unload();
		}

		texture = App->resources->RequestResource<ResourceTexture>(materialAsShared->GetOcclusionrUID()).lock();
		if (texture)
		{
			texture->Unload();
		}

		texture = App->resources->RequestResource<ResourceTexture>(materialAsShared->GetSpecularUID()).lock();
		if (texture)
		{
			texture->Unload();
		}
	}
}

void ComponentMaterial::UnloadTexture(TextureType textureType)
{
	std::shared_ptr<ResourceMaterial> materialAsShared = material.lock();

	if (materialAsShared)
	{
		std::shared_ptr<ResourceTexture> texture;
		switch (textureType)
		{
		case TextureType::DIFFUSE:
			texture = App->resources->RequestResource<ResourceTexture>(materialAsShared->GetDiffuseUID()).lock();
			if (texture)
			{
				texture->Unload();
			}
			break;
		case TextureType::NORMAL:
			texture = App->resources->RequestResource<ResourceTexture>(materialAsShared->GetNormalUID()).lock();
			if (texture)
			{
				texture->Unload();
			}
			break;
		case TextureType::OCCLUSION:
			texture = App->resources->RequestResource<ResourceTexture>(materialAsShared->GetOcclusionrUID()).lock();
			if (texture)
			{
				texture->Unload();
			}
			break;
		case TextureType::SPECULAR:
			texture = App->resources->RequestResource<ResourceTexture>(materialAsShared->GetSpecularUID()).lock();
			if (texture)
			{
				texture->Unload();
			}
			break;
		}
	}
}
