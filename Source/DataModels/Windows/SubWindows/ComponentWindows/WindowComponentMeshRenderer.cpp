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

WindowComponentMeshRenderer::WindowComponentMeshRenderer(ComponentMeshRenderer* component) :
	ComponentWindow("MESH RENDERER", component),
	inputMesh(std::make_unique<WindowMeshInput>(component)),
	inputMaterial(std::make_unique<WindowMaterialInput>(this)),
	inputTextureDiffuse(std::make_unique<WindowTextureInput>(this, TextureType::DIFFUSE)),
	inputTextureNormal(std::make_unique<WindowTextureInput>(this, TextureType::NORMAL)),
	//inputTextureSpecular(std::make_unique<WindowTextureInput>(this, TextureType::SPECULAR))
	inputTextureMetallic(std::make_unique<WindowTextureInput>(this, TextureType::METALLIC)),
	reset(false), newMaterial(false)
{
	InitMaterialValues();
}

WindowComponentMeshRenderer::~WindowComponentMeshRenderer()
{
}

void WindowComponentMeshRenderer::DrawWindowContents()
{
	DrawEnableAndDeleteComponent();
	ImGui::Text(""); //used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent
	ComponentMeshRenderer* asMeshRenderer = static_cast<ComponentMeshRenderer*>(component);

	if (asMeshRenderer)
	{
		std::shared_ptr<ResourceMesh> meshAsShared = asMeshRenderer->GetMesh();
		std::shared_ptr<ResourceMaterial> materialAsShared = asMeshRenderer->GetMaterial();
		static char* meshPath = (char*)("unknown");

		if (newMaterial)
		{
			asMeshRenderer->SetMaterial(material);
			asMeshRenderer->GetBatch()->ReserveModelSpace();
			newMaterial = false;
		}

		if (meshAsShared)
		{
			meshPath = (char*)(meshAsShared->GetLibraryPath().c_str());
		}
		else
		{
			meshPath = (char*)("unknown");
		}

		if (materialAsShared)
		{
			DrawSetMaterial();
		}
		else
		{
			DrawEmptyMaterial();
		}

		ImGui::InputText("##Mesh path", meshPath, 128);
		ImGui::SameLine();
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("GENERAL"))
			{
				UID draggedMeshUID = *(UID*)payload->Data; // Double pointer to keep track correctly
				//TODO this should be Asset Path of the asset not the UID (Because new filesystem cache)
				std::shared_ptr<ResourceMesh> newMesh =
					App->resources->SearchResource<ResourceMesh>(draggedMeshUID);
				//And then this should be RequestResource not SearchResource

				if (newMesh)
				{
					meshAsShared->Unload();
					asMeshRenderer->SetMesh(newMesh);
				}
			}

			ImGui::EndDragDropTarget();
		}
		

		bool showMeshBrowser;

		meshAsShared ? showMeshBrowser = false : showMeshBrowser = true;


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
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i ", (meshAsShared) ?
				meshAsShared->GetNumVertices() : 0);
			ImGui::TableNextColumn();
			ImGui::Text("Number of triangles: ");
			ImGui::TableNextColumn();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i ", (meshAsShared) ?
				meshAsShared->GetNumFaces() : 0); // faces = triangles

			ImGui::EndTable();
		}
	}
}

void WindowComponentMeshRenderer::DrawSetMaterial()
{
	ComponentMeshRenderer* asMeshRenderer = static_cast<ComponentMeshRenderer*>(component);

	if (asMeshRenderer)
	{
		bool updateMaterials = false;
		std::shared_ptr<ResourceMaterial> materialResource = asMeshRenderer->GetMaterial();

		if (materialResource)
		{
			ImGui::Text("");

			ImGui::Text("Name: ");
			ImGui::SameLine();
			ImGui::Text(materialResource->GetFileName().c_str());
			ImGui::SameLine();
			if (ImGui::Button("Remove Material"))
			{
				asMeshRenderer->GetBatch()->DeleteMaterial(asMeshRenderer);
				materialResource->Unload();
				asMeshRenderer->SetMaterial(nullptr);
				return;
			}

			ImGui::Text("");

			ImGui::Text("Diffuse Color:"); ImGui::SameLine();
			ImGui::ColorEdit3("##Diffuse Color", (float*)&colorDiffuse);

			//static float3 colorSpecular = materialResource->GetSpecularColor();
			//ImGui::Text("Specular Color:"); ImGui::SameLine();
			//if (ImGui::ColorEdit3("##Specular Color", (float*)&colorSpecular))
			//{
			//	materialResource->SetSpecularColor(colorSpecular);
			//}

			ImGui::Text("");

			static UID thisUID = UniqueID::GenerateUID();

			std::string removeButtonLabel = "No Texture";

			if (materialResource)
			{
				if (materialResource->GetDiffuse() || materialResource->GetNormal()
					|| materialResource->GetMetallicMap())
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
				//materialResource->SetSpecular(nullptr);
				materialResource->SetMetallicMap(nullptr);
				materialResource->SetChanged(true);
				updateMaterials = true;
			}

			//bool hasShininessAlpha = materialResource->HasShininessAlpha();
			//ImGui::Checkbox("Use specular Alpha as shininess", &hasShininessAlpha);
			//materialResource->SetShininessAlpha(hasShininessAlpha);

			//float shininess = materialResource->GetShininess();
			//ImGui::SliderFloat("Shininess", &shininess,
			//	0.1f, 512.f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
			//materialResource->SetShininess(shininess);
			ImGui::Separator();

			ImGui::Text("Diffuse Texture");
			if (diffuseTexture)
			{
				diffuseTexture->Load();
				ImGui::Image((void*)(intptr_t)diffuseTexture->GetGlTexture(), ImVec2(100, 100));
				if (ImGui::Button("Remove Texture Diffuse"))
				{
					diffuseTexture->Unload();
					diffuseTexture = nullptr;
					updateMaterials = true;
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
				ImGui::Image((void*)(intptr_t)metalicMap->GetGlTexture(), ImVec2(100, 100));
				if (ImGui::Button("Remove Texture Metallic"))
				{
					metalicMap->Unload();
					metalicMap = nullptr;
					updateMaterials = true;
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
				ImGui::Image((void*)(intptr_t)normalMap->GetGlTexture(), ImVec2(100, 100));
				if (ImGui::Button("Remove Texture Normal"))
				{
					normalMap->Unload();
					normalMap = nullptr;
					updateMaterials = true;
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
				updateMaterials = true;
			}
		}

		if (updateMaterials)
		{
			asMeshRenderer->GetBatch()->FillMaterial();
		}
	}
}

void WindowComponentMeshRenderer::DrawEmptyMaterial()
{
	ComponentMeshRenderer* asMeshRenderer = static_cast<ComponentMeshRenderer*>(component);

	if (asMeshRenderer)
	{
		if (asMeshRenderer->GetMaterial() == nullptr)
		{
			inputMaterial->DrawWindowContents();
		}
	}
}

void WindowComponentMeshRenderer::InitMaterialValues()
{
	ComponentMeshRenderer* asMaterial = static_cast<ComponentMeshRenderer*>(component);

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