#pragma warning (disable: 26495)
#pragma warning (disable: 4804)
#pragma warning (disable: 4312)

#include "ComponentMeshRenderer.h"

#include "ComponentTransform.h"
#include "Program/Program.h"

#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleProgram.h"
#include "FileSystem/ModuleResources.h"
#include "FileSystem/ModuleFileSystem.h"
#include "FileSystem/Json.h"

#include "Resources/ResourceMesh.h"
#include "Resources/ResourceMaterial.h"
#include "Resources/ResourceTexture.h"

#include "GameObject/GameObject.h"

#include <GL/glew.h>

#ifdef ENGINE
#include "DataModels/Resources/EditorResource/EditorResourceInterface.h"
#endif // ENGINE

ComponentMeshRenderer::ComponentMeshRenderer(const bool active, GameObject* owner)
	: Component(ComponentType::MESHRENDERER, active, owner, true)
{
}

ComponentMeshRenderer::ComponentMeshRenderer(const ComponentMeshRenderer& componentMeshRenderer):
	Component(componentMeshRenderer), 
	mesh(componentMeshRenderer.GetMesh()), material(componentMeshRenderer.GetMaterial())
{
}

ComponentMeshRenderer::~ComponentMeshRenderer()
{
	if (mesh)
		mesh->Unload();
}

void ComponentMeshRenderer::Update()
{

}

void ComponentMeshRenderer::Draw()
{
	//this should be in an EditorComponent class, or something of the like
	//but for now have it here
#ifdef ENGINE
	if (mesh && std::dynamic_pointer_cast<EditorResourceInterface>(mesh)->ToDelete())
	{
		mesh = nullptr;
	}
#endif // ENGINE

	if (this->IsMeshLoaded()) //pointer not empty
	{
		if (!mesh->IsLoaded())
		{
			mesh->Load();
		}

		Program* program = App->program->GetProgram(ProgramType::MESHSHADER);

		if (program)
		{
			program->Activate();
			const float4x4& view = App->camera->GetCamera()->GetViewMatrix();
			const float4x4& proj = App->camera->GetCamera()->GetProjectionMatrix();
			const float4x4& model =
				static_cast<ComponentTransform*>(GetOwner()
					->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();

			glUniformMatrix4fv(2, 1, GL_TRUE, (const float*)&model);
			glUniformMatrix4fv(1, 1, GL_TRUE, (const float*)&view);
			glUniformMatrix4fv(0, 1, GL_TRUE, (const float*)&proj);

			glBindVertexArray(mesh->GetVAO());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO());

			glDrawElements(GL_TRIANGLES, mesh->GetNumFaces() * 3, GL_UNSIGNED_INT, nullptr);

			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			program->Deactivate();
		}
	}
}

void ComponentMeshRenderer::DrawHighlight()
{
	if (IsMeshLoaded()) //pointer not empty
	{
		if (!mesh->IsLoaded())
		{
			mesh->Load();
		}

		float scale = 10.1f;
		Program* program = App->program->GetProgram(ProgramType::HIGHLIGHT);

		if (program)
		{
			program->Activate();
			const float4x4& view = App->camera->GetCamera()->GetViewMatrix();
			const float4x4& proj = App->camera->GetCamera()->GetProjectionMatrix();
			const float4x4& model =
				static_cast<ComponentTransform*>(GetOwner()
					->GetComponent(ComponentType::TRANSFORM))->GetGlobalMatrix();
			GLint programInUse;
			glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

			glUniformMatrix4fv(2, 1, GL_TRUE, (const float*)&model);
			glUniformMatrix4fv(1, 1, GL_TRUE, (const float*)&view);
			glUniformMatrix4fv(0, 1, GL_TRUE, (const float*)&proj);

			glBindVertexArray(mesh->GetVAO());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO());

			glDrawElements(GL_TRIANGLES, mesh->GetNumFaces() * 3, GL_UNSIGNED_INT, nullptr);

			glBindVertexArray(0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			program->Deactivate();
		}
	}
}

void ComponentMeshRenderer::SaveOptions(Json& meta)
{
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = (bool)active;
	meta["removed"] = (bool)canBeRemoved;

	UID uidMesh = 0;
	std::string assetPathMesh = "";

	if(mesh)
	{
		uidMesh = mesh->GetUID();
		assetPathMesh = mesh->GetAssetsPath();
	}

	meta["meshUID"] = (UID)uidMesh;
	meta["assetPathMesh"] = assetPathMesh.c_str();

	UID uidMaterial = 0;
	std::string assetPathMaterial = "";

	if (material)
	{
		uidMaterial = material->GetUID();
		assetPathMaterial = material->GetAssetsPath();
	}
	meta["materialUID"] = (UID)uidMaterial;
	meta["assetPathMaterial"] = assetPathMaterial.c_str();
}

void ComponentMeshRenderer::SaveUIDOfResourceToMeta(Json& meta,
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

void ComponentMeshRenderer::LoadOptions(Json& meta)
{
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];

#ifdef ENGINE
	std::string meshPath = meta["assetPathMesh"];
	bool resourceExists = meshPath != "" && App->fileSystem->Exists(meshPath.c_str());
	if (resourceExists)
	{
		std::shared_ptr<ResourceMesh> resourceMesh = App->resources->RequestResource
															<ResourceMesh>(meshPath);
		if (resourceMesh)
		{
			SetMesh(resourceMesh);
		}
	}

	std::string materialPath = meta["assetPathMaterial"];
	bool resourceExists = materialPath != "" && App->fileSystem->Exists(materialPath.c_str());
	if (resourceExists)
	{
		std::shared_ptr<ResourceMaterial> resourceMaterial = App->resources->RequestResource
															<ResourceMaterial>(materialPath);
		if (resourceMaterial)
		{
			SetMaterial(resourceMaterial);
		}
	}
#else
	UID uidMesh = meta["meshUID"];
	std::shared_ptr<ResourceMesh> resourceMesh = App->resources->SearchResource<ResourceMesh>(uidMesh);
	if (resourceMesh)
	{
		SetMesh(resourceMesh);
	}

	UID uidMaterial = meta["materialUID"];
	std::shared_ptr<ResourceMaterial> resourceMaterial = App->resources->SearchResource<ResourceMaterial>(uidMaterial);
	if (resourceMaterial)
	{
		SetMaterial(resourceMaterial);
	}
#endif
}

void ComponentMeshRenderer::SetMesh(const std::shared_ptr<ResourceMesh>& newMesh)
{
	mesh = newMesh;

	if (mesh)
	{
		mesh->Load();
		GetOwner()->Encapsule(mesh->GetVertices().data(), mesh->GetNumVertices());
	}
}

void ComponentMeshRenderer::SetMaterial(const std::shared_ptr<ResourceMaterial>& newMaterial)
{
	material = newMaterial;

	if (material)
	{
		material->Load();
	}
}
void ComponentMeshRenderer::UnloadTextures()
{
	if (material)
	{
		std::shared_ptr<ResourceTexture> texture = material->GetDiffuse();
		if (texture)
		{
			texture->Unload();
		}

		texture = material->GetNormal();
		if (texture)
		{
			texture->Unload();
		}

		texture = material->GetOcclusion();
		if (texture)
		{
			texture->Unload();
		}

		texture = material->GetSpecular();
		if (texture)
		{
			texture->Unload();
		}
		texture = material->GetMetallicMap();
		if (texture)
		{
			texture->Unload();
		}
	}
}

void ComponentMeshRenderer::UnloadTexture(TextureType textureType)
{
	if (material)
	{
		std::shared_ptr<ResourceTexture> texture;
		switch (textureType)
		{
		case TextureType::DIFFUSE:
			texture = material->GetDiffuse();
			if (texture)
			{
				texture->Unload();
			}
			break;
		case TextureType::NORMAL:
			texture = material->GetNormal();
			if (texture)
			{
				texture->Unload();
			}
			break;
		case TextureType::OCCLUSION:
			texture = material->GetOcclusion();
			if (texture)
			{
				texture->Unload();
			}
			break;
			case TextureType::SPECULAR:
				texture = material->GetSpecular();
				if (texture)
				{
					texture->Unload();
				}
				break;
		case TextureType::METALLIC:
			texture = material->GetMetallicMap();
			if (texture)
			{
				texture->Unload();
			}
			break;
		}
	}
}

const float3& ComponentMeshRenderer::GetDiffuseColor() const {
	return material->GetDiffuseColor();
}

const float3& ComponentMeshRenderer::GetSpecularColor() const {
	return material->GetSpecularColor();
}

const float ComponentMeshRenderer::GetShininess() const {
	return material->GetShininess();
}

const float ComponentMeshRenderer::GetNormalStrenght() const {
	return material->GetNormalStrength();
}

const float ComponentMeshRenderer::GetSmoothness() const
{
	return material->GetSmoothness();
}

const float ComponentMeshRenderer::GetMetalness() const
{
	return material->GetMetalness();
}

const bool ComponentMeshRenderer::HasShininessAlpha() const {
	return material->HasShininessAlpha();
}

const bool ComponentMeshRenderer::HasMetallicAlpha() const
{
	return material->HasMetallicAlpha();
}

void ComponentMeshRenderer::SetDiffuseColor(float3& diffuseColor)
{
	this->material->SetDiffuseColor(diffuseColor);
}

void ComponentMeshRenderer::SetSpecularColor(float3& specularColor)
{
	this->material->SetSpecularColor(specularColor);
}

void ComponentMeshRenderer::SetShininess(float shininess)
{
	this->material->SetShininess(shininess);
}

void ComponentMeshRenderer::SetNormalStrenght(float normalStrength)
{
	this->material->SetNormalStrength(normalStrength);
}

void ComponentMeshRenderer::SetSmoothness(float smoothness)
{
	this->material->SetSmoothness(smoothness);
}

void ComponentMeshRenderer::SetMetalness(float metalness)
{
	this->material->SetMetalness(metalness);
}

void ComponentMeshRenderer::SetHasShininessAlpha(bool hasShininessAlpha)
{
	this->material->SetShininess(hasShininessAlpha);
}

void ComponentMeshRenderer::SetMetallicAlpha(bool metallicAlpha)
{
	this->material->SetMetallicAlpha(metallicAlpha);
}
