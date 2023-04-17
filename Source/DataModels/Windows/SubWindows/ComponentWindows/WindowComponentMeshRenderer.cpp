#include "WindowComponentMeshRenderer.h"

#include "DataModels/Components/ComponentMeshRenderer.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"

#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowMeshInput.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowTextureInput.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowMaterialInput.h"

#include "DataModels/Resources/ResourceMesh.h"
#include "DataModels/Resources/ResourceMaterial.h"
#include "DataModels/Resources/ResourceTexture.h"

const std::vector<std::string> WindowComponentMeshRenderer::shaderTypes 
												= { "Default", "Specular" };
const std::vector<std::string> WindowComponentMeshRenderer::renderModes 
											= { "Opaque", "Transparent" };


WindowComponentMeshRenderer::WindowComponentMeshRenderer(ComponentMeshRenderer* component) :
	ComponentWindow("MESH RENDERER", component), 
	inputMesh(std::make_unique<WindowMeshInput>(component)), 
	inputMaterial(std::make_unique<WindowMaterialInput>(component)),
	inputTextureDiffuse(std::make_unique<WindowTextureInput>(this, TextureType::DIFFUSE)),
	inputTextureNormal(std::make_unique<WindowTextureInput>(this, TextureType::NORMAL)),
	inputTextureMetallic(std::make_unique<WindowTextureInput>(this, TextureType::METALLIC)),
	inputTextureSpecular(std::make_unique<WindowTextureInput>(this, TextureType::SPECULAR))
{
	InitMaterialValues();
}

WindowComponentMeshRenderer::~WindowComponentMeshRenderer()
{
}

void WindowComponentMeshRenderer::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();

	//used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent
	ImGui::Text(""); 

	ComponentMeshRenderer* asMeshRenderer = 
		static_cast<ComponentMeshRenderer*>(component);

	if (asMeshRenderer)
	{
		std::shared_ptr<ResourceMesh> meshAsShared = asMeshRenderer->GetMesh();

		char* meshPath;
		bool showMeshBrowser = true;
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_None;

		if (meshAsShared)
		{
			//this should not be done, see issue #240
			meshPath = (char*)(meshAsShared->GetLibraryPath().c_str());
			showMeshBrowser = false;
		}
		else
		{
			meshPath = _strdup("unknown");
			flags |= ImGuiInputTextFlags_ReadOnly;
		}

		ImGui::InputText("##Mesh path", meshPath, 128);
		ImGui::SameLine();

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload 
				= ImGui::AcceptDragDropPayload("GENERAL"))
			{
				// Double pointer to keep track correctly
				UID draggedMeshUID = *static_cast<UID*>(payload->Data);

				/*TODO: this should be Asset Path of the asset not the UID
				(Because new filesystem cache)*/
				std::shared_ptr<ResourceMesh> newMesh =
					App->resources->SearchResource<ResourceMesh>
														(draggedMeshUID);
				//And then this should be RequestResource not SearchResource

				if (newMesh)
				{
					meshAsShared->Unload();
					asMeshRenderer->SetMesh(newMesh);
				}
			}

			ImGui::EndDragDropTarget();
		}

		if (showMeshBrowser)
		{
			inputMesh->DrawWindowContents();
		}
		else if (ImGui::Button("Remove Mesh"))
		{
			meshAsShared->Unload();
			asMeshRenderer->SetMesh(nullptr);
		}

		if (ImGui::BeginTable("##GeometryTable", 2))
		{
			ImGui::TableNextColumn();
			ImGui::Text("Number of vertices: ");
			ImGui::TableNextColumn();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i ", 
				(meshAsShared) ? meshAsShared->GetNumVertices() : 0);
			ImGui::TableNextColumn();
			ImGui::Text("Number of triangles: ");
			ImGui::TableNextColumn();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i ",
				(meshAsShared) ? meshAsShared->GetNumFaces() : 0);

			ImGui::EndTable();
		}

		if (asMeshRenderer->GetMaterial())
		{
			DrawSetMaterial();
		}
		else
		{
			DrawEmptyMaterial();
		}
	}
}

void WindowComponentMeshRenderer::DrawSetMaterial()
{
	ComponentMeshRenderer* asMeshRenderer = 
		static_cast<ComponentMeshRenderer*>(component);

	if (asMeshRenderer)
	{
		const std::shared_ptr<ResourceMaterial>& materialResource = 
			asMeshRenderer->GetMaterial();

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
				asMeshRenderer->SetMaterial(nullptr);
				return;
			}

			ImGui::Text("");

			const char* currentShaderType =
				shaderTypes[currentShaderTypeIndex].c_str();

			ImGui::Text("Shader type:"); ImGui::SameLine();

			if (ImGui::BeginCombo("##Shader type", currentShaderType))
			{
				for (unsigned int i = 0; i < shaderTypes.size(); ++i)
				{
					const bool isSelected = currentShaderTypeIndex == i;

					if (ImGui::Selectable(shaderTypes[i].c_str(), isSelected))
					{
						currentShaderTypeIndex = i;
					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::Text("");

			if (!isTransparent)
			{
				currentTransparentIndex = 0;
			}
			else
			{
				currentTransparentIndex = 1;
			}

			const char* currentType = 
				renderModes[currentTransparentIndex].c_str();

			ImGui::Text("Render Mode:"); ImGui::SameLine();

			if (ImGui::BeginCombo("##Render mode", currentType))
			{
				for (unsigned int i = 0; i < renderModes.size(); ++i)
				{
					const bool isSelected = currentTransparentIndex == i;

					if (ImGui::Selectable(renderModes[i].c_str(), isSelected))
					{
						currentTransparentIndex = i;

						if (renderModes[i] == "Opaque")
						{
							isTransparent = false;
						}
						else if (renderModes[i] == "Transparent")
						{
							isTransparent = true;
						}

					}

					if (isSelected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}

			ImGui::Text("Diffuse Color:"); ImGui::SameLine();
			ImGui::ColorEdit4("##Diffuse Color", (float*)&colorDiffuse);

			if (currentShaderTypeIndex == 1)
			{
				const float3& colorSpecular = 
					materialResource->GetSpecularColor();

				ImGui::Text("Specular Color:"); ImGui::SameLine();

				if (ImGui::ColorEdit3("##Specular Color", 
					(float*)&colorSpecular))
				{
					materialResource->SetSpecularColor(colorSpecular);
				}
			}

			ImGui::Text("");

			static UID thisUID = UniqueID::GenerateUID();

			std::string removeButtonLabel = "No Texture";

			if (materialResource)
			{
				if (materialResource->GetDiffuse() 
					|| materialResource->GetNormal()
					|| materialResource->GetMetallic() 
					|| materialResource->GetSpecular())
				{
					removeButtonLabel = "Remove Textures";
				}

			}

			if (ImGui::Button(removeButtonLabel.c_str()) && materialResource)
			{
				asMeshRenderer->UnloadTextures();
				
				materialResource->SetDiffuse(nullptr);
				materialResource->SetNormal(nullptr);
				materialResource->SetOcclusion(nullptr);
				materialResource->SetMetallic(nullptr);
				materialResource->SetSpecular(nullptr);
				
				materialResource->SetChanged(true);
			}

			ImGui::Separator();

			ImGui::Text("Diffuse Texture");

			if (diffuseTexture)
			{
				diffuseTexture->Load();

				ImGui::Image((void*)(intptr_t)diffuseTexture->GetGlTexture(),
					ImVec2(100, 100));

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

			if (currentShaderTypeIndex == 0)
			{
				ImGui::Text("Metallic Texture");
				if (metallicMap)
				{
					metallicMap->Load();
					ImGui::Image((void*)(intptr_t)metallicMap->GetGlTexture(),
						ImVec2(100, 100));

					if (ImGui::Button("Remove Texture Metallic"))
					{
						metallicMap->Unload();
						metallicMap = nullptr;
					}
				}
				else
				{
					inputTextureMetallic->DrawWindowContents();
				}
			}

			ImGui::DragFloat("Smoothness", &smoothness, 0.01f, 0.0f, 1.0f);

			if (currentShaderTypeIndex == 0)
			{
				ImGui::DragFloat("Metallic", &metalness, 0.01f, 0.0f, 1.0f);
			}

			ImGui::Separator();

			if (currentShaderTypeIndex == 1)
			{
				ImGui::Text("Specular Texture");

				if (specularMap)
				{
					specularMap->Load();
					ImGui::Image((void*)(intptr_t)specularMap->GetGlTexture(),
						ImVec2(100, 100));

					if (ImGui::Button("Remove Texture Specular"))
					{
						specularMap->Unload();
						specularMap = nullptr;
					}
				}
				else
				{
					inputTextureSpecular->DrawWindowContents();
				}
			}

			ImGui::Text("Normal Texture");

			if (normalMap)
			{
				normalMap->Load();
				ImGui::Image((void*)(intptr_t)normalMap->GetGlTexture(),
					ImVec2(100, 100));

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

			ImGui::DragFloat("Normal Strength", &normalStrength, 0.01f, 0.0f,
				std::numeric_limits<float>::max());

			ImGui::Text("");
			ImGui::SameLine(ImGui::GetWindowWidth() - 120);

			if (ImGui::Button("Reset"))
			{
				InitMaterialValues();
			}

			ImGui::SameLine(ImGui::GetWindowWidth() - 70);

			if (ImGui::Button("Apply"))
			{
				materialResource->SetShaderType(currentShaderTypeIndex);
				materialResource->SetDiffuseColor(colorDiffuse);
				materialResource->SetDiffuse(diffuseTexture);
				materialResource->SetMetallic(metallicMap);
				materialResource->SetNormal(normalMap);
				materialResource->SetSmoothness(smoothness);
				materialResource->SetMetalness(metalness);
				materialResource->SetNormalStrength(normalStrength);
				materialResource->SetTransparent(isTransparent);
				materialResource->SetChanged(true);
				App->resources->ReimportResource(materialResource->GetUID());
			}
		}
	}
}

void WindowComponentMeshRenderer::DrawEmptyMaterial()
{
	const ComponentMeshRenderer* asMeshRenderer = 
		static_cast<ComponentMeshRenderer*>(component);

	if (asMeshRenderer && !asMeshRenderer->GetMaterial())
	{
		inputMaterial->DrawWindowContents();
	}
}

void WindowComponentMeshRenderer::InitMaterialValues()
{
	ComponentMeshRenderer* asMeshRenderer = 
		static_cast<ComponentMeshRenderer*>(component);

	if (asMeshRenderer)
	{
		const std::shared_ptr<ResourceMaterial>& materialResource = 
			asMeshRenderer->GetMaterial();

		if (materialResource)
		{
			colorDiffuse = materialResource->GetDiffuseColor();
			diffuseTexture = materialResource->GetDiffuse();
			metallicMap = materialResource->GetMetallic();
			specularMap = materialResource->GetSpecular();
			normalMap = materialResource->GetNormal();
			smoothness = materialResource->GetSmoothness();
			metalness = materialResource->GetMetalness();
			normalStrength = materialResource->GetNormalStrength();
			isTransparent = materialResource->GetTransparent();
		}
	}
}