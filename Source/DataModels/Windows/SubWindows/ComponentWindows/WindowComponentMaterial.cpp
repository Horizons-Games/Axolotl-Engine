#include "WindowComponentMaterial.h"

#include "DataModels/Components/ComponentMaterial.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowTextureInput.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowMaterialInput.h"

#include "DataModels/Resources/ResourceMaterial.h"
#include "DataModels/Resources/ResourceTexture.h"

WindowComponentMaterial::WindowComponentMaterial(ComponentMaterial* component) :
	ComponentWindow("MATERIAL", component), 
	inputMaterial(std::make_unique<WindowMaterialInput>(component)),
	inputTextureDiffuse(std::make_unique<WindowTextureInput>(component, TextureType::DIFFUSE)),
	inputTextureNormal(std::make_unique<WindowTextureInput>(component, TextureType::NORMAL)),
	inputTextureSpecular(std::make_unique<WindowTextureInput>(component, TextureType::SPECULAR))
{
}

WindowComponentMaterial::~WindowComponentMaterial()
{
}

void WindowComponentMaterial::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	ComponentMaterial* asMaterial = static_cast<ComponentMaterial*>(component);

	if (asMaterial)
	{
		if (asMaterial->GetMaterial())
		{
			DrawSetMaterial();
		}
		else
		{
			DrawEmptyMaterial();
		}
	}
}

void WindowComponentMaterial::DrawSetMaterial()
{
	ComponentMaterial* asMaterial = static_cast<ComponentMaterial*>(component);

	if (asMaterial)
	{
		std::shared_ptr<ResourceMaterial> materialResource = asMaterial->GetMaterial();
		if (materialResource)
		{
			ImGui::Text("");

			char name[20] = "Texture";
			ImGui::InputText("Texture Name", name, 20);

			ImGui::Text("");

			static float3 colorDiffuse = asMaterial->GetDiffuseColor();
			ImGui::Text("Diffuse Color:"); ImGui::SameLine();
			if (ImGui::ColorEdit3("##Diffuse Color", (float*)&colorDiffuse))
			{
				asMaterial->SetDiffuseColor(colorDiffuse);
			}

			static float3 colorSpecular = asMaterial->GetSpecularColor();
			ImGui::Text("Specular Color:"); ImGui::SameLine();
			if (ImGui::ColorEdit3("##Specular Color", (float*)&colorSpecular))
			{
				asMaterial->SetSpecularColor(colorSpecular);
			}

			ImGui::Text("");

			static UID thisUID = UniqueID::GenerateUID();

			std::string removeButtonLabel = "No Texture";

			if (materialResource)
			{
				if (materialResource->GetDiffuseUID() || materialResource->GetNormalUID()
					|| materialResource->GetSpecularUID())
				{
					removeButtonLabel = "Remove Textures";
				}

			}

			if (ImGui::Button(removeButtonLabel.c_str()) && materialResource)
			{
				asMaterial->UnloadTextures();
				
				UID uidNull = 0;
				materialResource->SetDiffuseUID(uidNull);
				materialResource->SetNormalUID(uidNull);
				materialResource->SetOcclusionUID(uidNull);
				materialResource->SetSpecularUID(uidNull);
				
				materialResource->SetChanged(true);
				
				asMaterial->diffuseUID = 0;
				asMaterial->normalUID = 0;
				asMaterial->occlusionUID = 0;
				asMaterial->specularUID = 0;
			}

			ImGui::Checkbox("Use specular Alpha as shininess", &(asMaterial->hasShininessAlpha));
			ImGui::SliderFloat("Shininess", &(asMaterial->shininess),
				0.1f, 200.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
			ImGui::Separator();

			ImGui::Text("Diffuse Texture");
			bool showTextureBrowserDiffuse = true;
			std::shared_ptr<ResourceTexture> texture;
			if (materialResource)
			{
				if (materialResource->GetDiffuseUID())
				{
					 texture = 
						 App->resources->RequestResource<ResourceTexture>(materialResource->GetDiffuseUID()).lock();
					if (texture)
					{
						ImGui::Image((void*)(intptr_t)texture->GetGlTexture(), ImVec2(100, 100));
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
				if (ImGui::Button("Remove Texture Diffuse") && materialResource->GetDiffuseUID())
				{
					asMaterial->UnloadTexture(TextureType::DIFFUSE);

					UID uidNull = 0;
					materialResource->SetDiffuseUID(uidNull);
					asMaterial->diffuseUID = 0;
				}
			}

			ImGui::Separator();

			ImGui::Text("Specular Texture");
			bool showTextureBrowserSpecular = true;
			if (materialResource && materialResource->GetSpecularUID())
			{
				texture =
					App->resources->RequestResource<ResourceTexture>(materialResource->GetSpecularUID()).lock();
				if (texture)
				{
					ImGui::Image((void*)(intptr_t)texture->GetGlTexture(), ImVec2(100, 100));
				}

				showTextureBrowserSpecular = false;
			}

			if (showTextureBrowserSpecular)
			{
				inputTextureSpecular->DrawWindowContents();
			}
			else
			{
				if (ImGui::Button("Remove Texture Specular") && materialResource->GetSpecularUID())
				{
					asMaterial->UnloadTexture(TextureType::SPECULAR);

					UID uidNull = 0;
					materialResource->SetSpecularUID(uidNull);
					asMaterial->specularUID = 0;
				}
			}

			ImGui::Separator();

			ImGui::Text("Normal Texture");
			bool showTextureBrowserNormal = true;
			if (materialResource && materialResource->GetNormalUID())
			{
					texture =
						App->resources->RequestResource<ResourceTexture>(materialResource->GetNormalUID()).lock();
					if (texture)
					{
						ImGui::Image((void*)(intptr_t)texture->GetGlTexture(), ImVec2(100, 100));
					}

					showTextureBrowserNormal = false;
			}

			if (showTextureBrowserNormal)
			{
				inputTextureNormal->DrawWindowContents();
			}
			else if (ImGui::Button("Remove Texture Normal"))
			{
				if (materialResource->GetNormalUID())
				{
					asMaterial->UnloadTexture(TextureType::NORMAL);

					UID uidNull = 0;
					materialResource->SetNormalUID(uidNull);
					asMaterial->normalUID = 0;
				}
			}
			ImGui::SliderFloat("Normal", &(asMaterial->normalStrength),
				0.0f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

			ImGui::Text("");
		}
	}
}

void WindowComponentMaterial::DrawEmptyMaterial()
{
	ComponentMaterial* asMaterial = static_cast<ComponentMaterial*>(component);

	if (asMaterial)
	{
		if (asMaterial->GetMaterial() == nullptr)
		{
			inputMaterial->DrawWindowContents();
		}
	}
}
