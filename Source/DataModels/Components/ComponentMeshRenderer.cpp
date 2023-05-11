#include "ComponentMeshRenderer.h"

#include "ComponentTransform.h"

#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleRender.h"
#include "ModuleProgram.h"

#include "Program/Program.h"
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

#endif

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
	{
		batch->DeleteComponent(this);
		mesh->Unload();
	}
	if (material)
	{
		material->Unload();
	}
}

void ComponentMeshRenderer::DrawHighlight() const
{
	if (IsMeshLoaded())
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
			const float4x4& proj = 
							App->camera->GetCamera()->GetProjectionMatrix();
			const float4x4& model =
				static_cast<ComponentTransform*>(GetOwner()
					->GetComponent(ComponentType::TRANSFORM))
														->GetGlobalMatrix();

			GLint programInUse;

			glGetIntegerv(GL_CURRENT_PROGRAM, &programInUse);

			glUniformMatrix4fv(2, 1, GL_TRUE, (const float*)&model);
			glUniformMatrix4fv(1, 1, GL_TRUE, (const float*)&view);
			glUniformMatrix4fv(0, 1, GL_TRUE, (const float*)&proj);

			glBindVertexArray(mesh->GetVAO());
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetEBO());

			glDrawElements(GL_TRIANGLES, mesh->GetNumFaces() * 3, 
													GL_UNSIGNED_INT, nullptr);

			glBindVertexArray(0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			program->Deactivate();
		}
	}
}

void ComponentMeshRenderer::SaveOptions(Json& meta)
{
	meta["type"] = GetNameByType(type).c_str();
	meta["active"] = static_cast<bool>(active);
	meta["removed"] = static_cast<bool>(canBeRemoved);

	UID uid = 0;
	std::string assetPath = "";

	if(mesh)
	{
		uid = mesh->GetUID();
		assetPath = mesh->GetAssetsPath();
	}

	meta["meshUID"] = static_cast<UID>(uid);
	meta["assetPathMesh"] = assetPath.c_str();

	if (material)
	{
		uid = material->GetUID();
		assetPath = material->GetAssetsPath();
	}

	meta["materialUID"] = static_cast<UID>(uid);
	meta["assetPathMaterial"] = assetPath.c_str();
}

void ComponentMeshRenderer::LoadOptions(Json& meta)
{
	type = GetTypeByName(meta["type"]);
	active = static_cast<bool>(meta["active"]);
	canBeRemoved = static_cast<bool>(meta["removed"]);

#ifdef ENGINE
	std::string path = meta["assetPathMaterial"];
	bool materialExists = path != "" && App->fileSystem->Exists(path.c_str());

	if (materialExists)
	{
		std::shared_ptr<ResourceMaterial> resourceMaterial =
			App->resources->RequestResource<ResourceMaterial>(path);

		if (resourceMaterial)
		{
			SetMaterial(resourceMaterial);
		}
	}
	 path = meta["assetPathMesh"];
	bool meshExists = path != "" && App->fileSystem->Exists(path.c_str());

	if (meshExists)
	{
		std::shared_ptr<ResourceMesh> resourceMesh =
			App->resources->RequestResource<ResourceMesh>(path);

		if (resourceMesh)
		{
			SetMesh(resourceMesh);
		}
	}
#else

	UID uidMaterial = meta["materialUID"];
	std::shared_ptr<ResourceMaterial> resourceMaterial =
		App->resources->SearchResource<ResourceMaterial>(uidMaterial);

	if (resourceMaterial)
	{
		SetMaterial(resourceMaterial);
	}

	UID uidMesh = meta["meshUID"];
	std::shared_ptr<ResourceMesh> resourceMesh =
		App->resources->SearchResource<ResourceMesh>(uidMesh);

	if (resourceMesh)
	{
		SetMesh(resourceMesh);
	}


#endif
}
void ComponentMeshRenderer::SetMesh(const std::shared_ptr<ResourceMesh>& newMesh)
{
	mesh = newMesh;

	if (mesh)
	{
		mesh->Load();
		ComponentTransform* transform =
			static_cast<ComponentTransform*>
			(GetOwner()->GetComponent(ComponentType::TRANSFORM));

		transform->Encapsule
		(mesh->GetVertices().data(), mesh->GetNumVertices());
		App->renderer->GetBatchManager()->AddComponent(this);
	}
	else
	{
		batch->DeleteComponent(this);
		batch = nullptr;
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

		/*texture = material->GetSpecular();
		if (texture)
		{
			texture->Unload();
		}*/
		texture = material->GetMetallic();
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
			/*case TextureType::SPECULAR:
				texture = material->GetSpecular();
				if (texture)
				{
					texture->Unload();
				}
				break;*/
		case TextureType::METALLIC:
			texture = material->GetMetallic();
			if (texture)
			{
				texture->Unload();
			}
			break;
		}
	}
}

// Common attributes (setters)
void ComponentMeshRenderer::SetDiffuseColor(float4& diffuseColor)
{
	this->material->SetDiffuseColor(diffuseColor);
}

void ComponentMeshRenderer::SetDiffuse(const std::shared_ptr<ResourceTexture>& diffuse)
{
	this->material->SetDiffuse(diffuse);
}

void ComponentMeshRenderer::SetNormal(const std::shared_ptr<ResourceTexture>& normal)
{
	this->material->SetNormal(normal);
}

void ComponentMeshRenderer::SetMetallic(const std::shared_ptr<ResourceTexture>& metallic)
{
	this->material->SetMetallic(metallic);
}

void ComponentMeshRenderer::SetSpecular(const std::shared_ptr<ResourceTexture>& specular)
{
	this->material->SetSpecular(specular);
}

void ComponentMeshRenderer::SetShaderType(unsigned int shaderType)
{
	this->material->SetShaderType(shaderType);
}

void ComponentMeshRenderer::SetSmoothness(float smoothness)
{
	this->material->SetSmoothness(smoothness);
}

void ComponentMeshRenderer::SetNormalStrength(float normalStrength)
{
	this->material->SetNormalStrength(normalStrength);
}

// Default shader attributes (setters)
void ComponentMeshRenderer::SetMetalness(float metalness)
{
	this->material->SetMetalness(metalness);
}

// Specular shader attributes (setters)
void ComponentMeshRenderer::SetSpecularColor(float3& specularColor)
{
	this->material->SetSpecularColor(specularColor);
}

void ComponentMeshRenderer::SetTransparent(bool isTransparent)
{
	this->material->SetTransparent(isTransparent);
}

void ComponentMeshRenderer::RemoveFromBatch()
{
	batch->DeleteComponent(this);
}

std::vector<ComponentMeshRenderer*> ComponentMeshRenderer::ChangeOfBatch()
{
	return batch->ChangeBatch(this);
}

const unsigned int& ComponentMeshRenderer::GetShaderType() const
{
	return material->GetShaderType();
}


// Common attributes (getters)

const float4& ComponentMeshRenderer::GetDiffuseColor() const
{
	return material->GetDiffuseColor();
}

const float ComponentMeshRenderer::GetSmoothness() const
{
	return material->GetSmoothness();
}

const float ComponentMeshRenderer::GetNormalStrenght() const
{
	return material->GetNormalStrength();
}

// Default shader attributes (getters)

const float ComponentMeshRenderer::GetMetalness() const
{
	return material->GetMetalness();
}

// Specular shader attributes (getters)

const float3& ComponentMeshRenderer::GetSpecularColor() const
{
	return material->GetSpecularColor();
}

const bool ComponentMeshRenderer::IsTransparent() const
{
	return material->IsTransparent();
}

const std::shared_ptr<ResourceTexture>& ComponentMeshRenderer::GetDiffuse() const
{
	return material->GetDiffuse();;
}

const std::shared_ptr<ResourceTexture>& ComponentMeshRenderer::GetNormal() const
{
	return material->GetNormal();
}

const std::shared_ptr<ResourceTexture>& ComponentMeshRenderer::GetOcclusion() const
{
	return material->GetOcclusion();
}

const std::shared_ptr<ResourceTexture>& ComponentMeshRenderer::GetMetallic() const
{
	return material->GetMetallic();
}

const std::shared_ptr<ResourceTexture>& ComponentMeshRenderer::GetSpecular() const
{
	return material->GetSpecular();
}