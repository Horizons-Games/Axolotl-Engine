#include "ComponentMaterial.h"

#include "Application.h"
#include "ModuleProgram.h"
#include "ModuleEngineCamera.h"
#include "FileSystem/ModuleResources.h"

#include "GameObject/GameObject.h"

#include "Resources/ResourceTexture.h"

#include <GL/glew.h>

ComponentMaterial::ComponentMaterial(bool active, GameObject* owner)
	: Component(ComponentType::MATERIAL, active, owner)
{
}

ComponentMaterial::~ComponentMaterial()
{
}

void ComponentMaterial::Update()
{
	unsigned int program = App->program->GetProgram();
	glUniform3f(glGetUniformLocation(program, "material.diffuse_color"), diffuseColor.x, diffuseColor.y, diffuseColor.z);
	std::shared_ptr<ResourceTexture> texture = textureDiffuse.lock();
	if (texture)
	{
		glUniform1i(glGetUniformLocation(program, "material.has_diffuse_map"), 1);
		glUniform1i(glGetUniformLocation(program, "material.diffuse_map"), texture->GetGlTexture());
		glActiveTexture(GL_TEXTURE0 + texture->GetGlTexture());
		glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
	}
	else 
	{
		glUniform1i(glGetUniformLocation(program, "material.has_diffuse_map"), 0);
	}
	texture = textureSpecular.lock();
	if (texture)
	{
		glUniform1i(glGetUniformLocation(program, "material.has_specular_map"), 1);
		glUniform1i(glGetUniformLocation(program, "material.specular_map"), texture->GetGlTexture());
		glActiveTexture(GL_TEXTURE0 + texture->GetGlTexture());
		glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
	}
	else
	{
		glUniform1i(glGetUniformLocation(program, "material.has_specular_map"), 0);
	}
	glUniform3f(glGetUniformLocation(program, "material.specular_color"), specularColor.x, specularColor.y, specularColor.z);
	glUniform1f(glGetUniformLocation(program, "material.shininess"), shininess);
	glUniform1f(glGetUniformLocation(program, "material.shininess_alpha"), hasShininessAlpha);

	//move to main component(?)
	float3 viewPos = App->engineCamera->GetPosition();
	glUniform3f(glGetUniformLocation(program, "viewPos"), viewPos.x, viewPos.y, viewPos.z);
}

void ComponentMaterial::Display()
{
}

void ComponentMaterial::SetDiffuseUID(UID& diffuseUID)
{
	this->diffuseUID = diffuseUID;
	LoadTexture(TextureType::DIFFUSE);
}

void ComponentMaterial::SetNormalUID(UID& normalUID)
{
	this->normalUID = normalUID;
	LoadTexture(TextureType::NORMAL);
}

void ComponentMaterial::SetOcclusionUID(UID& occlusionUID)
{
	this->occlusionUID = occlusionUID;
	LoadTexture(TextureType::OCCLUSION);
}

void ComponentMaterial::SetSpecularUID(UID& specularUID)
{
	this->specularUID = specularUID;
	LoadTexture(TextureType::SPECULAR);
}

void ComponentMaterial::LoadTexture(TextureType textureType)
{
	std::shared_ptr<ResourceTexture> texture;
	switch (textureType)
	{
	case TextureType::DIFFUSE:
		textureDiffuse = std::static_pointer_cast<ResourceTexture>(App->resources->RequestResource(diffuseUID).lock());
		texture = textureDiffuse.lock();
		if (texture)
		{
			texture->Load();
			hasDiffuse = true;
		}
		break;
	case TextureType::NORMAL:
		textureNormal = std::static_pointer_cast<ResourceTexture>(App->resources->RequestResource(normalUID).lock());
		texture = textureNormal.lock();
		if (texture)
		{
			texture->Load();
			hasNormal = true;
		}
		break;
	case TextureType::OCCLUSION:
		textureOcclusion = std::static_pointer_cast<ResourceTexture>(App->resources->RequestResource(occlusionUID).lock());
		texture = textureOcclusion.lock();
		if (texture)
		{
			texture->Load();
			hasOcclusion = true;
		}
		break;
	case TextureType::SPECULAR:
		textureSpecular = std::static_pointer_cast<ResourceTexture>(App->resources->RequestResource(specularUID).lock());
		texture = textureSpecular.lock();
		if (texture)
		{
			texture->Load();
			hasSpecular = true;
		}
		break;
	}
}
