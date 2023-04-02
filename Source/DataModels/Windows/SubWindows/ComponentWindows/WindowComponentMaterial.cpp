#include "WindowComponentMaterial.h"

#include "DataModels/Components/ComponentMaterial.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowMaterialInput.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowTextureInput.h"

#include "DataModels/Resources/ResourceMaterial.h"
#include "DataModels/Resources/ResourceTexture.h"

WindowComponentMaterial::WindowComponentMaterial(ComponentMaterial* component) :
	ComponentWindow("MATERIAL", component),
	inputMaterial(std::make_unique<WindowMaterialInput>(component)),
	inputTextureDiffuse(std::make_unique<WindowTextureInput>(this, TextureType::DIFFUSE)),
	inputTextureNormal(std::make_unique<WindowTextureInput>(this, TextureType::NORMAL)),
	// inputTextureSpecular(std::make_unique<WindowTextureInput>(component, TextureType::SPECULAR)),
	inputTextureMetallic(std::make_unique<WindowTextureInput>(this, TextureType::METALLIC))
{
	InitMaterialValues();
}

WindowComponentMaterial::~WindowComponentMaterial()
{
}

void WindowComponentMaterial::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text(""); // used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent
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

			ImGui::Text("Name: ");
			ImGui::SameLine();
			ImGui::Text(materialResource->GetFileName().c_str());
			ImGui::SameLine();
			if (ImGui::Button("Remove Material"))
			{
				materialResource->Unload();
				asMaterial->SetMaterial(nullptr);
				return;
			}

			ImGui::Text("");

			ImGui::Text("Diffuse Color:");
			ImGui::SameLine();
			ImGui::ColorEdit3("##Diffuse Color", (float*) &colorDiffuse);

			/*static float3 colorSpecular = materialResource->GetSpecularColor();
			ImGui::Text("Specular Color:"); ImGui::SameLine();
			if (ImGui::ColorEdit3("##Specular Color", (float*)&colorSpecular))
			{
				materialResource->SetSpecularColor(colorSpecular);
			}

			ImGui::Text("");

			bool hasShininessAlpha = materialResource->HasShininessAlpha();
			ImGui::Checkbox("Use specular Alpha as shininess", &hasShininessAlpha);
			materialResource->SetShininessAlpha(hasShininessAlpha);

			float shininess = materialResource->GetShininess();
			ImGui::SliderFloat("Shininess", &shininess,
				0.1f, 512.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
			materialResource->SetShininess(shininess);
			ImGui::Separator();*/

			ImGui::Text("Diffuse Texture");
			if (diffuseTexture)
			{
				diffuseTexture->Load();
				ImGui::Image((void*) (intptr_t) diffuseTexture->GetGlTexture(), ImVec2(100, 100));
				if (ImGui::Button("Remove Texture Diffuse"))
				{
					diffuseTexture->Unload();
					diffuseTexture = nullptr;
				}
			}
			else
			{
				inputTextureDiffuse->DrawWindowContents();
			}

			ImGui::Separator();

			ImGui::Text("Metallic Texture");
			if (metalicMap)
			{
				metalicMap->Load();
				ImGui::Image((void*) (intptr_t) metalicMap->GetGlTexture(), ImVec2(100, 100));
				if (ImGui::Button("Remove Texture Metallic"))
				{
					metalicMap->Unload();
					metalicMap = nullptr;
				}
			}
			else
			{
				inputTextureMetallic->DrawWindowContents();
			}

			ImGui::DragFloat("Smoothness", &smoothness, 0.01f, 0.0f, 1.0f);

			ImGui::DragFloat("Metallic", &metalness, 0.01f, 0.0f, 1.0f);

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

			ImGui::Separator();

			ImGui::Text("Normal Texture");
			if (normalMap)
			{
				normalMap->Load();
				ImGui::Image((void*) (intptr_t) normalMap->GetGlTexture(), ImVec2(100, 100));
				if (ImGui::Button("Remove Texture Normal"))
				{
					normalMap->Unload();
					normalMap = nullptr;
				}
			}
			else
			{
				inputTextureNormal->DrawWindowContents();
			}

			ImGui::DragFloat("Normal Strength", &normalStrength, 0.01f, 0.0f, std::numeric_limits<float>::max());
			ImGui::Text("");
			ImGui::SameLine(ImGui::GetWindowWidth() - 120);
			if (ImGui::Button("Reset"))
			{
				InitMaterialValues();
			}
			ImGui::SameLine(ImGui::GetWindowWidth() - 70);
			if (ImGui::Button("Apply"))
			{
				materialResource->SetDiffuseColor(colorDiffuse);
				materialResource->SetDiffuse(diffuseTexture);
				materialResource->SetMetallicMap(metalicMap);
				materialResource->SetNormal(normalMap);
				materialResource->SetSmoothness(smoothness);
				materialResource->SetMetalness(metalness);
				materialResource->SetNormalStrength(normalStrength);
				materialResource->SetChanged(true);
				App->resources->ReimportResource(materialResource->GetUID());
			}
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

void WindowComponentMaterial::InitMaterialValues()
{
	ComponentMaterial* asMaterial = static_cast<ComponentMaterial*>(component);

	if (asMaterial)
	{
		std::shared_ptr<ResourceMaterial> materialResource = asMaterial->GetMaterial();
		if (materialResource)
		{
			colorDiffuse = materialResource->GetDiffuseColor();
			diffuseTexture = materialResource->GetDiffuse();
			metalicMap = materialResource->GetMetallicMap();
			normalMap = materialResource->GetNormal();
			smoothness = materialResource->GetSmoothness();
			metalness = materialResource->GetMetalness();
			normalStrength = materialResource->GetNormalStrength();
		}
	}
}
