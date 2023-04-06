#pragma warning (disable: 4804)
#pragma warning (disable: 4312)

#include "ComponentMaterial.h"



#include <GL/glew.h>

#ifdef ENGINE
#include "DataModels/Resources/EditorResource/EditorResourceInterface.h"
#endif // ENGINE
#include "DataModels/Program/Program.h"


ComponentMaterial::ComponentMaterial(bool active, GameObject* owner)
	: Component(ComponentType::MATERIAL, active, owner, true)
{
}

ComponentMaterial::ComponentMaterial(const ComponentMaterial& componentMaterial) :
	Component(componentMaterial), material(componentMaterial.GetMaterial())
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
			const float3& diffuseColor = material->GetDiffuseColor();
			glUniform3f(3, diffuseColor.x, diffuseColor.y, diffuseColor.z); //diffuse_color
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

void ComponentMaterial::SaveOptions(Json& meta)
{
	
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
#ifdef ENGINE
	std::string path = meta["assetPathMaterial"];
	bool resourceExists = path != "" && App->fileSystem->Exists(path.c_str());
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
	if(resourceMaterial)
	{
		SetMaterial(resourceMaterial);
	}
#endif
}

void ComponentMaterial::SetMaterial(const std::shared_ptr<ResourceMaterial>& newMaterial)
{
	material = newMaterial;
	
	if (material)
	{
		material->Load();
	}
}



const float3& ComponentMaterial::GetDiffuseColor() const {
	return material->GetDiffuseColor();
}

/*const float3& ComponentMaterial::GetSpecularColor() const {
	return material->GetSpecularColor();
}

const float ComponentMaterial::GetShininess() const {
	return material->GetShininess();
}*/

const float ComponentMaterial::GetNormalStrenght() const {
	return material->GetNormalStrength();
}

const float ComponentMaterial::GetSmoothness() const
{
	return material->GetSmoothness();
}

const float ComponentMaterial::GetMetalness() const
{
	return material->GetMetalness();
}

/*const bool ComponentMaterial::HasShininessAlpha() const {
	return material->HasShininessAlpha();
}*/

const bool ComponentMaterial::HasMetallicAlpha() const
{
	return material->HasMetallicAlpha();
}

void ComponentMaterial::SetDiffuseColor(float3& diffuseColor)
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

void ComponentMaterial::SetNormalStrenght(float normalStrength)
{
	this->material->SetNormalStrength(normalStrength);
}

void ComponentMaterial::SetSmoothness(float smoothness)
{
	this->material->SetSmoothness(smoothness);
}

void ComponentMaterial::SetMetalness(float metalness)
{
	this->material->SetMetalness(metalness);
}

/*void ComponentMaterial::SetHasShininessAlpha(bool hasShininessAlpha)
{
	this->material->SetShininess(hasShininessAlpha);
}*/

void ComponentMaterial::SetMetallicAlpha(bool metallicAlpha)
{
	this->material->SetMetallicAlpha(metallicAlpha);
}
