#include "WindowComponentMaterial.h"

#include "DataModels/Components/ComponentMaterial.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowTextureInput.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowMaterialInput.h"

#include "DataModels/Resources/ResourceMaterial.h"
#include "DataModels/Resources/ResourceTexture.h"

WindowComponentMaterial::WindowComponentMaterial(const std::weak_ptr<ComponentMaterial>& component) :
	ComponentWindow("MATERIAL", component)
{
	inputMaterial = std::make_unique<WindowMaterialInput>(component);
	inputTextureDiffuse = std::make_unique<WindowTextureInput>(component, TextureType::DIFFUSE);
	inputTextureNormal = std::make_unique<WindowTextureInput>(component, TextureType::NORMAL);
	inputTextureSpecular = std::make_unique<WindowTextureInput>(component, TextureType::SPECULAR);
}

void WindowComponentMaterial::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	std::shared_ptr<ComponentMaterial> asSharedMaterial =
		std::dynamic_pointer_cast<ComponentMaterial>(component.lock());

	if (asSharedMaterial)
	{
		std::shared_ptr<ResourceMaterial> materialResource = asSharedMaterial->GetMaterial().lock();
		if (materialResource)
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
	std::shared_ptr<ComponentMaterial> asSharedMaterial =
		std::dynamic_pointer_cast<ComponentMaterial>(component.lock());

	if (asSharedMaterial)
	{
		std::shared_ptr<ResourceMaterial> materialResource = asSharedMaterial->GetMaterial().lock();
		if (materialResource)
		{
			ImGui::Text("");

			char name[20] = "Texture";
			ImGui::InputText("Texture Name", name, 20);

			ImGui::Text("");

			static float3 colorDiffuse = asSharedMaterial->GetDiffuseColor();
			ImGui::Text("Diffuse Color:"); ImGui::SameLine();
			if (ImGui::ColorEdit3("##Diffuse Color", (float*)&colorDiffuse))
				asSharedMaterial->SetDiffuseColor(colorDiffuse);

			static float3 colorSpecular = asSharedMaterial->GetSpecularColor();
			ImGui::Text("Specular Color:"); ImGui::SameLine();
			if (ImGui::ColorEdit3("##Specular Color", (float*)&colorSpecular))
				asSharedMaterial->SetSpecularColor(colorSpecular);

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
				asSharedMaterial->UnloadTextures();
				
				UID uidNull = 0;
				materialResource->SetDiffuseUID(uidNull);
				materialResource->SetNormalUID(uidNull);
				materialResource->SetOcclusionUID(uidNull);
				materialResource->SetSpecularUID(uidNull);
				
				materialResource->SetChanged(true);
				
				asSharedMaterial->diffuseUID = 0;
				asSharedMaterial->normalUID = 0;
				asSharedMaterial->occlusionUID = 0;
				asSharedMaterial->specularUID = 0;
			}

			ImGui::Checkbox("Use specular Alpha as shininess", &(asSharedMaterial->hasShininessAlpha));
			ImGui::SliderFloat("Shininess", &(asSharedMaterial->shininess),
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
					asSharedMaterial->UnloadTexture(TextureType::DIFFUSE);

					UID uidNull = 0;
					materialResource->SetDiffuseUID(uidNull);
					asSharedMaterial->diffuseUID = 0;
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
					asSharedMaterial->UnloadTexture(TextureType::SPECULAR);

					UID uidNull = 0;
					materialResource->SetSpecularUID(uidNull);
					asSharedMaterial->specularUID = 0;
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
					asSharedMaterial->UnloadTexture(TextureType::NORMAL);

					UID uidNull = 0;
					materialResource->SetNormalUID(uidNull);
					asSharedMaterial->normalUID = 0;
				}
			}
			ImGui::SliderFloat("Normal", &(asSharedMaterial->normalStrength),
				0.0f, 1.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);

			ImGui::Text("");
		}
	}
}

void WindowComponentMaterial::DrawEmptyMaterial()
{
	std::shared_ptr<ComponentMaterial> asSharedMaterial =
		std::dynamic_pointer_cast<ComponentMaterial>(component.lock());

	if (asSharedMaterial)
	{
		if (asSharedMaterial->GetMaterial().expired())
		{
			inputMaterial->DrawWindowContents();
		}
	}
}
