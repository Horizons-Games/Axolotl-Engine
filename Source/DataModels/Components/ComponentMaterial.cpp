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
#include "imgui.h"

ComponentMaterial::ComponentMaterial(bool active, const std::shared_ptr<GameObject>& owner)
	: Component(ComponentType::MATERIAL, active, owner, true)
{
	inputMaterial = std::make_unique<WindowMaterialInput>(this);
	inputTextureDiffuse = std::make_unique<WindowTextureInput>(this, TextureType::DIFFUSE);
	inputTextureNormal = std::make_unique<WindowTextureInput>(this, TextureType::NORMAL);
	inputTextureSpecular = std::make_unique<WindowTextureInput>(this, TextureType::SPECULAR);
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
	std::shared_ptr<Program> program = App->program->GetProgram(ProgramType::MESHSHADER).lock();
	if(program)
	{
		program->Activate();

		std::shared_ptr<ResourceMaterial> materialAsShared = material.lock();

		if(materialAsShared) 
		{
			std::shared_ptr<ResourceTexture> texture = App->resources->
											RequestResource<ResourceTexture>(materialAsShared->GetDiffuseUID()).lock();

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

			texture = App->resources->RequestResource<ResourceTexture>(materialAsShared->GetSpecularUID()).lock();
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

			texture = App->resources->RequestResource<ResourceTexture>(materialAsShared->GetNormalUID()).lock();
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

void ComponentMaterial::Display()
{
	std::shared_ptr<ResourceMaterial> materialAsShared = material.lock();

	if (ImGui::CollapsingHeader("MATERIAL", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (materialAsShared)
		{
			DisplaySetMaterial();
		}
		else
		{
			DisplayEmptyMaterial();
		}
	}
	ImGui::Separator();
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

void ComponentMaterial::DisplaySetMaterial()
{
	std::shared_ptr<ResourceMaterial> materialAsShared = material.lock();

	if (materialAsShared)
	{
		ImGui::Text("");
		if (materialAsShared)
			ImGui::Text(std::to_string(materialAsShared->GetUID()).c_str());

		if (ImGui::Button("Remove material"))
		{
			material = std::weak_ptr<ResourceMaterial>();
		}

		char name[20] = "Texture";
		ImGui::InputText("Texture Name", name, 20);

		ImGui::Text("");

		static float3 colorDiffuse = GetDiffuseColor();
		ImGui::Text("Diffuse Color:"); ImGui::SameLine();
		if (ImGui::ColorEdit3("##Diffuse Color", (float*)&colorDiffuse))
			SetDiffuseColor(colorDiffuse);

		static float3 colorSpecular = GetSpecularColor();
		ImGui::Text("Specular Color:"); ImGui::SameLine();
		if (ImGui::ColorEdit3("##Specular Color", (float*)&colorSpecular))
			SetSpecularColor(colorSpecular);

		ImGui::Text("");

		static UID thisUID = UniqueID::GenerateUID();

		std::string removeButtonLabel = "No Texture";

		if (materialAsShared)
		{
			bool haveTextures = false;
			haveTextures += materialAsShared->GetDiffuseUID();
			haveTextures += materialAsShared->GetNormalUID();
			haveTextures += materialAsShared->GetSpecularUID();

			if (haveTextures)
			{
				removeButtonLabel = "Remove Textures";
			}

		}

		if (ImGui::Button(removeButtonLabel.c_str()))
		{
			if (materialAsShared)
			{
				UnloadTextures();

				UID uidNull = 0;
				materialAsShared->SetDiffuseUID(uidNull);
				materialAsShared->SetNormalUID(uidNull);
				materialAsShared->SetOcclusionUID(uidNull);
				materialAsShared->SetSpecularUID(uidNull);

				App->resources->ReimportResource(materialAsShared->GetUID());

				diffuseUID = 0;
				normalUID = 0;
				occlusionUID = 0;
				specularUID = 0;
			}
		}

		ImGui::Checkbox("Use specular Alpha as shininess", &hasShininessAlpha);
		ImGui::SliderFloat("Shininess", &shininess, 0.1f, 200.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
		ImGui::Separator();

		ImGui::Text("Diffuse Texture");
		bool showTextureBrowserDiffuse = true;
		if (materialAsShared)
		{
			if (materialAsShared->GetDiffuseUID())
			{
				std::shared_ptr<ResourceTexture> texture =
					App->resources->RequestResource<ResourceTexture>(materialAsShared->GetDiffuseUID()).lock();
				if (texture)
				{
					ImGui::Image((void*)texture->GetGlTexture(), ImVec2(100, 100));
				}

				showTextureBrowserDiffuse = false;
			}
		}

		if (showTextureBrowserDiffuse)
		{
			inputTextureDiffuse->DrawWindowContents();
		}
		else
		{
			if (ImGui::Button("Remove Texture Diffuse"))
			{
				if (materialAsShared->GetDiffuseUID())
				{
					UnloadTexture(TextureType::DIFFUSE);

					UID uidNull = 0;
					materialAsShared->SetDiffuseUID(uidNull);
					diffuseUID = 0;
				}
			}
		}

		ImGui::Separator();

		ImGui::Text("Specular Texture");
		bool showTextureBrowserSpecular = true;
		if (materialAsShared)
		{
			if (materialAsShared->GetSpecularUID())
			{
				std::shared_ptr<ResourceTexture> texture =
					App->resources->RequestResource<ResourceTexture>(materialAsShared->GetSpecularUID()).lock();
				if (texture)
				{
					ImGui::Image((void*)texture->GetGlTexture(), ImVec2(100, 100));
				}

				showTextureBrowserSpecular = false;
			}
		}

		if (showTextureBrowserSpecular)
		{
			inputTextureSpecular->DrawWindowContents();
		}
		else
		{
			if (ImGui::Button("Remove Texture Specular"))
			{
				if (materialAsShared->GetSpecularUID())
				{
					UnloadTexture(TextureType::SPECULAR);

					UID uidNull = 0;
					materialAsShared->SetSpecularUID(uidNull);
					specularUID = 0;
				}
			}
		}

		ImGui::Separator();

		ImGui::Text("Normal Texture");
		bool showTextureBrowserNormal = true;
		if (materialAsShared)
		{
			if (materialAsShared->GetNormalUID())
			{
				std::shared_ptr<ResourceTexture> texture =
					App->resources->RequestResource<ResourceTexture>(materialAsShared->GetNormalUID()).lock();
				if (texture)
				{
					ImGui::Image((void*)texture->GetGlTexture(), ImVec2(100, 100));
				}

				showTextureBrowserNormal = false;
			}
		}

		if (showTextureBrowserNormal)
		{
			inputTextureNormal->DrawWindowContents();
		}
		else
		{
			if (ImGui::Button("Remove Texture Normal"))
			{
				if (materialAsShared->GetNormalUID())
				{
					UnloadTexture(TextureType::NORMAL);

					UID uidNull = 0;
					materialAsShared->SetNormalUID(uidNull);
					normalUID = 0;
				}
			}
		}
		ImGui::SliderFloat("Normal", &normalStrength, 0.0f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

		ImGui::Text("");
	}
}

void ComponentMaterial::DisplayEmptyMaterial()
{
	if (material.expired())
	{
		inputMaterial->DrawWindowContents();
	}
}
