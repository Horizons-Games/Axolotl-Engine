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
	//inputTextureSpecular(std::make_unique<WindowTextureInput>(component, TextureType::SPECULAR)),
	inputTextureMetallic(std::make_unique<WindowTextureInput>(component, TextureType::METALLIC))
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

			static float3 colorDiffuse = materialResource->GetDiffuseColor();
			ImGui::Text("Diffuse Color:"); ImGui::SameLine();
			if (ImGui::ColorEdit3("##Diffuse Color", (float*)&colorDiffuse))
			{
				materialResource->SetDiffuseColor(colorDiffuse);
			}

			/*static float3 colorSpecular = materialResource->GetSpecularColor();
			ImGui::Text("Specular Color:"); ImGui::SameLine();
			if (ImGui::ColorEdit3("##Specular Color", (float*)&colorSpecular))
			{
				materialResource->SetSpecularColor(colorSpecular);
			}

			ImGui::Text("");*/

			static UID thisUID = UniqueID::GenerateUID();

			std::string removeButtonLabel = "No Texture";

			if (materialResource)
			{
				if (materialResource->GetDiffuse() || materialResource->GetNormal()
					|| //materialResource->GetSpecular()
					materialResource->GetMetallicMap())
				{
					removeButtonLabel = "Remove Textures";
				}

			}

			if (ImGui::Button(removeButtonLabel.c_str()) && materialResource)
			{
				asMaterial->UnloadTextures();
				
				materialResource->SetDiffuse(nullptr);
				materialResource->SetNormal(nullptr);
				materialResource->SetOcclusion(nullptr);
				//materialResource->SetSpecular(nullptr);
				materialResource->SetMetallicMap(nullptr);
				
				materialResource->SetChanged(true);
			}

			/*bool hasShininessAlpha = materialResource->HasShininessAlpha();
			ImGui::Checkbox("Use specular Alpha as shininess", &hasShininessAlpha);
			materialResource->SetShininessAlpha(hasShininessAlpha);

			float shininess = materialResource->GetShininess();
			ImGui::SliderFloat("Shininess", &shininess,
				0.1f, 512.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
			materialResource->SetShininess(shininess);
			ImGui::Separator();*/

			ImGui::Text("Diffuse Texture");
			bool showTextureBrowserDiffuse = true;
			std::shared_ptr<ResourceTexture> texture;
			if (materialResource)
			{
				if (materialResource->GetDiffuse())
				{
					texture = materialResource->GetDiffuse();
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
				if (ImGui::Button("Remove Texture Diffuse") && materialResource->GetDiffuse())
				{
					asMaterial->UnloadTexture(TextureType::DIFFUSE);

					materialResource->SetDiffuse(nullptr);
				}
			}

			ImGui::Separator();

			ImGui::Text("Metallic Texture");
			bool showMetallicBrowserDiffuse = true;
			if (materialResource)
			{
				if (materialResource->GetMetallicMap())
				{
					texture = materialResource->GetMetallicMap();
					if (texture)
					{
						ImGui::Image((void*)(intptr_t)texture->GetGlTexture(), ImVec2(100, 100));
					}

					showMetallicBrowserDiffuse = false;
				}
			}

			if (showMetallicBrowserDiffuse)
			{
				inputTextureMetallic->DrawWindowContents();
			}
			else
			{
				if (ImGui::Button("Remove Texture Metallic") && materialResource->GetMetallicMap())
				{
					asMaterial->UnloadTexture(TextureType::METALLIC);

					materialResource->SetMetallicMap(nullptr);
				}
			}

			float smoothness = asMaterial->GetSmoothness();
			if (ImGui::DragFloat("Smoothness", &smoothness,
				0.01f, 0.0f, 1.0f))
			{
				asMaterial->SetSmoothness(smoothness);
			}

			
			float metalness = asMaterial->GetMetalness();
			if (ImGui::DragFloat("Metallic", &metalness,
				0.01f, 0.0f, 1.0f))
			{
				asMaterial->SetMetalness(metalness);
			}
			
			ImGui::Separator();

			/*ImGui::Text("Specular Texture");
			bool showTextureBrowserSpecular = true;
			if (materialResource && materialResource->GetSpecular())
			{
				texture =
					std::dynamic_pointer_cast<ResourceTexture>(materialResource->GetSpecular());
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
				if (ImGui::Button("Remove Texture Specular") && materialResource->GetSpecular())
				{
					asMaterial->UnloadTexture(TextureType::SPECULAR);

					materialResource->SetSpecular(nullptr);
				}
			}

			ImGui::Separator();*/

			ImGui::Text("Normal Texture");
			bool showTextureBrowserNormal = true;
			if (materialResource && materialResource->GetNormal())
			{
					texture = materialResource->GetNormal();
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
				if (materialResource->GetNormal())
				{
					asMaterial->UnloadTexture(TextureType::NORMAL);

					materialResource->SetNormal(nullptr);
				}
			}

			float normalStrength = asMaterial->GetNormalStrenght();
			if (ImGui::DragFloat("Normal Strength", &normalStrength,
				0.01f, 0.0f, std::numeric_limits<float>::max()))
			{
				asMaterial->SetNormalStrenght(normalStrength);
			}

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
