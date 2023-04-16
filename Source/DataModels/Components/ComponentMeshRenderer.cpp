#pragma warning (disable: 26495)
#pragma warning (disable: 4804)
#pragma warning (disable: 4312)

#include "ComponentMeshRenderer.h"

#include "ComponentTransform.h"

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

#include "GL/glew.h"


#ifdef ENGINE
#include "DataModels/Resources/EditorResource/EditorResourceInterface.h"
#endif // ENGINE

ComponentMeshRenderer::ComponentMeshRenderer(const bool active, GameObject* owner)
	: Component(ComponentType::MESHRENDERER, active, owner, true)
{
}

ComponentMeshRenderer::ComponentMeshRenderer(const ComponentMeshRenderer& componentMeshRenderer):
	Component(componentMeshRenderer), mesh(componentMeshRenderer.GetMesh())
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
	Program* program = App->program->GetProgram(ProgramType::MESHSHADER);

	if (program)
	{
		program->Activate();
		DrawMaterial(program);
		DrawMeshes(program);
	}
	program->Deactivate();
}
void ComponentMeshRenderer::DrawMeshes(Program* program)
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
void ComponentMeshRenderer::DrawMaterial(Program* program)
{
	if (program)
	{
		program->Activate();

		//this should be in an EditorComponent class, or something of the like
		//but for now have it here
#ifdef ENGINE
		if (material && std::dynamic_pointer_cast<EditorResourceInterface>(material)->ToDelete())
		{
			material = nullptr;
		}
#endif // ENGINE

		if (material)
		{
			const float4& diffuseColor = material->GetDiffuseColor();
			glUniform4f(3, diffuseColor.x, diffuseColor.y, diffuseColor.z, diffuseColor.w); //diffuse_color
			std::shared_ptr<ResourceTexture> texture = material->GetDiffuse();
			if (texture)
			{
				if (!texture->IsLoaded())
				{
					texture->Load();
				}

				glUniform1i(5, 1); //has_diffuse_map

				glActiveTexture(GL_TEXTURE5);
				glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
			}
			else
			{
				glUniform1i(5, 0); //has_diffuse_map
			}

			/*const float3& specularColor = material->GetSpecularColor();
			glUniform3f(4, specularColor.x, specularColor.y, specularColor.z); //specular_color
			texture = material->GetSpecular();
			if (texture)
			{
				if (!texture->IsLoaded())
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
				glUniform1i(8, 1); //has_specular_map
				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
			}
			else
			{
				glUniform1i(8, 0); //has_specular_map
			}*/

			texture = std::dynamic_pointer_cast<ResourceTexture>(material->GetNormal());
			if (texture)
			{
				if (!texture->IsLoaded())
				{
					texture->Load();
				}

				glActiveTexture(GL_TEXTURE6);
				glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
				glUniform1f(4, material->GetNormalStrength()); //normal_strength
				glUniform1i(6, 1); //has_normal_map
			}
			else
			{
				glUniform1i(6, 0); //has_normal_map
			}

			/*glUniform1f(5, material->GetShininess()); //shininess
			glUniform1f(9, material->HasShininessAlpha()); //shininess_alpha
			*/
			glUniform1f(7, material->GetSmoothness());
			glUniform1i(8, material->HasMetallicAlpha());
			glUniform1f(9, material->GetMetalness());

			texture = material->GetMetallicMap();
			if (texture)
			{
				if (!texture->IsLoaded())
				{
					texture->Load();
				}

				glUniform1i(10, 1); //has_metallic_map
				glActiveTexture(GL_TEXTURE7);
				glBindTexture(GL_TEXTURE_2D, texture->GetGlTexture());
			}
			else
			{
				glUniform1i(10, 0); //has_metallic_map
			}

			float3 viewPos = App->camera->GetCamera()->GetPosition();
			program->BindUniformFloat3("viewPos", viewPos);
		}

		program->Deactivate();
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
	std::string assetPath = "";

	if(mesh)
	{
		uidMesh = mesh->GetUID();
		assetPath = mesh->GetAssetsPath();
	}

	meta["meshUID"] = (UID)uidMesh;
	meta["assetPathMesh"] = assetPath.c_str();

	//Material
	UID uidMaterial = 0;
	assetPath = "";

	if (material)
	{
		uidMaterial = material->GetUID();
		assetPath = material->GetAssetsPath();
	}
	meta["materialUID"] = (UID)uidMaterial;
	meta["assetPathMaterial"] = assetPath.c_str();
}

void ComponentMeshRenderer::LoadOptions(Json& meta)
{
	type = GetTypeByName(meta["type"]);
	active = (bool)meta["active"];
	canBeRemoved = (bool)meta["removed"];

#ifdef ENGINE
	std::string path = meta["assetPathMesh"];
	bool resourceExists = path != "" && App->fileSystem->Exists(path.c_str());
	if (resourceExists)
	{
		std::shared_ptr<ResourceMesh> resourceMesh = App->resources->RequestResource<ResourceMesh>(path);
		if (resourceMesh)
		{
			SetMesh(resourceMesh);
		}
	}
#else
	UID uidMesh = meta["meshUID"];
	std::shared_ptr<ResourceMesh> resourceMesh = App->resources->SearchResource<ResourceMesh>(uidMesh);
	if (resourceMesh)
	{
		SetMesh(resourceMesh);
	}
#endif

	//Material
#ifdef ENGINE
	path = meta["assetPathMaterial"];
	resourceExists = path != "" && App->fileSystem->Exists(path.c_str());
	if (resourceExists)
	{
		std::shared_ptr<ResourceMaterial> resourceMaterial = App->resources->RequestResource<ResourceMaterial>(path);
		if (resourceMaterial)
		{
			SetMaterial(resourceMaterial);
		}
	}
#else
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
		ComponentTransform* transform = static_cast<ComponentTransform*>(GetOwner()->GetComponent(ComponentType::TRANSFORM));
		transform->Encapsule(mesh->GetVertices().data(), mesh->GetNumVertices());
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

		//texture = material->GetSpecular();
		//if (texture)
		//{
		//	texture->Unload();
		//}
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
		//case TextureType::SPECULAR:
		//	texture = material->GetSpecular();
		//	if (texture)
		//	{
		//		texture->Unload();
		//	}
		//	break;
		}
	}
}

const float4& ComponentMeshRenderer::GetDiffuseColor() const {
	return material->GetDiffuseColor();
}

/*const float3& ComponentMaterial::GetSpecularColor() const {
	return material->GetSpecularColor();
}
const float ComponentMaterial::GetShininess() const {
	return material->GetShininess();
}*/

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

/*const bool ComponentMaterial::HasShininessAlpha() const {
	return material->HasShininessAlpha();
}*/

const bool ComponentMeshRenderer::HasMetallicAlpha() const
{
	return material->HasMetallicAlpha();
}

void ComponentMeshRenderer::SetDiffuseColor(float4& diffuseColor)
{
	this->material->SetDiffuseColor(diffuseColor);
}

/*void ComponentMaterial::SetSpecularColor(float3& specularColor)
{
	this->material->SetSpecularColor(specularColor);
}
void ComponentMaterial::SetShininess(float shininess)
{
	this->material->SetShininess(shininess);
}*/

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

/*void ComponentMaterial::SetHasShininessAlpha(bool hasShininessAlpha)
{
	this->material->SetShininess(hasShininessAlpha);
}*/

void ComponentMeshRenderer::SetMetallicAlpha(bool metallicAlpha)
{
	this->material->SetMetallicAlpha(metallicAlpha);
}