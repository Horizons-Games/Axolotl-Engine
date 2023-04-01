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
	ComponentWindow("MESH RENDERER", component), inputMesh(std::make_unique<WindowMeshInput>(component)), inputMaterial(std::make_unique<WindowMaterialInput>(component)),
	inputTextureDiffuse(std::make_unique<WindowTextureInput>(component, TextureType::DIFFUSE)),
	inputTextureNormal(std::make_unique<WindowTextureInput>(component, TextureType::NORMAL)),
	//inputTextureSpecular(std::make_unique<WindowTextureInput>(component, TextureType::SPECULAR))
	inputTextureMetallic(std::make_unique<WindowTextureInput>(component, TextureType::METALLIC))
{
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
		std::shared_ptr<ResourceMaterial> materialResource = asMeshRenderer->GetMaterial();
		asMeshRenderer->GetBatch()->UpdateMaterial();
		asMeshRenderer->GetBatch()->updateMaterial = false;

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

			static float3 colorDiffuse = materialResource->GetDiffuseColor();
			ImGui::Text("Diffuse Color:"); ImGui::SameLine();
			if (ImGui::ColorEdit3("##Diffuse Color", (float*)&colorDiffuse))
			{
				materialResource->SetDiffuseColor(colorDiffuse);
				asMeshRenderer->GetBatch()->updateMaterial = true;
			}

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
				asMeshRenderer->GetBatch()->updateMaterial = true;
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
					asMeshRenderer->UnloadTexture(TextureType::DIFFUSE);

					materialResource->SetDiffuse(nullptr);
					asMeshRenderer->GetBatch()->updateMaterial = true;
				}
			}

			ImGui::Separator();

			ImGui::Text("Mettalic Texture");
			bool showTextureBrowserSpecular = true;
			if (materialResource && materialResource->GetMetallicMap())
			{
				texture =
					std::dynamic_pointer_cast<ResourceTexture>(materialResource->GetMetallicMap());
				if (texture)
				{
					ImGui::Image((void*)(intptr_t)texture->GetGlTexture(), ImVec2(100, 100));
				}

				showTextureBrowserSpecular = false;
			}

			if (showTextureBrowserSpecular)
			{
				inputTextureMetallic->DrawWindowContents();
			}
			else
			{
				if (ImGui::Button("Remove Texture Metallic") && materialResource->GetMetallicMap())
				{
					asMeshRenderer->UnloadTexture(TextureType::METALLIC);

					materialResource->SetMetallicMap(nullptr);
					asMeshRenderer->GetBatch()->updateMaterial = true;
				}
			}

			ImGui::Separator();

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
					asMeshRenderer->UnloadTexture(TextureType::NORMAL);

					materialResource->SetNormal(nullptr);
					asMeshRenderer->GetBatch()->updateMaterial = true;
				}
			}

			float normalStrength = asMeshRenderer->GetNormalStrenght();
			if (ImGui::DragFloat("Normal Strength", &normalStrength,
				0.01f, 0.0001f, std::numeric_limits<float>::max()))
			{
				asMeshRenderer->SetNormalStrenght(normalStrength);
			}

			ImGui::Text("");
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

