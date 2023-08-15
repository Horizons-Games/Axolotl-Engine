#include "StdAfx.h"

#include "WindowComponentMeshRenderer.h"

#include "DataModels/Components/ComponentMeshRenderer.h"

#include "Application.h"
#include "FileSystem/ModuleResources.h"
#include "FileSystem/UIDGenerator.h"
#include "ModuleRender.h"

#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowMaterialInput.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowMeshInput.h"
#include "DataModels/Windows/EditorWindows/ImporterWindows/WindowTextureInput.h"

#include "DataModels/Batch/BatchManager.h"
#include "DataModels/Batch/GeometryBatch.h"

#include "DataModels/Resources/ResourceMaterial.h"
#include "DataModels/Resources/ResourceMesh.h"
#include "DataModels/Resources/ResourceTexture.h"

#define BLOOM_MAX_VALUE 5.f
#define BLOOM_MIN_VALUE 0.05f

const std::vector<std::string> WindowComponentMeshRenderer::shaderTypes = { "Default", "Specular" };
const std::vector<std::string> WindowComponentMeshRenderer::renderModes = { "Opaque", "Transparent" };

WindowComponentMeshRenderer::WindowComponentMeshRenderer(ComponentMeshRenderer* component) :
	ComponentWindow("MESH RENDERER", component),
	inputMesh(std::make_unique<WindowMeshInput>(component)),
	inputMaterial(std::make_unique<WindowMaterialInput>(this)),
	inputTextureDiffuse(std::make_unique<WindowTextureInput>(this, TextureType::DIFFUSE)),
	inputTextureNormal(std::make_unique<WindowTextureInput>(this, TextureType::NORMAL)),
	inputTextureMetallic(std::make_unique<WindowTextureInput>(this, TextureType::METALLIC)),
	inputTextureSpecular(std::make_unique<WindowTextureInput>(this, TextureType::SPECULAR)),
	inputTextureEmission(std::make_unique<WindowTextureInput>(this, TextureType::EMISSION)),
	reset(false),
	newMaterial(false)
{
	InitMaterialValues();
}

WindowComponentMeshRenderer::~WindowComponentMeshRenderer()
{
	ResetMaterialValues();
}

std::shared_ptr<ResourceMaterial> WindowComponentMeshRenderer::GetMaterial() const
{
	std::shared_ptr<ResourceMaterial> materialResource = static_cast<ComponentMeshRenderer*>(component)->GetMaterial();
	return materialResource;
}

void WindowComponentMeshRenderer::SetMaterial(const std::shared_ptr<ResourceMaterial>& material)
{
	ComponentMeshRenderer* asMeshRenderer = static_cast<ComponentMeshRenderer*>(component);
	asMeshRenderer->SetMaterial(material);
	if (asMeshRenderer->GetBatch())
	{
		asMeshRenderer->GetBatch()->ReserveModelSpace();
		asMeshRenderer->RemoveFromBatch();
	}

	App->GetModule<ModuleRender>()->GetBatchManager()->AddComponent(asMeshRenderer);

	InitMaterialValues();
}

void WindowComponentMeshRenderer::DrawWindowContents()
{
	if (changed)
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(75, 25, 25, 255));
	}
	else
	{
		ImGui::PushStyleColor(ImGuiCol_ChildBg, IM_COL32(0, 0, 0, 0));
	}
	ImGui::BeginChild("##Window");
	DrawEnableAndDeleteComponent();

	// used to ignore the ImGui::SameLine called in DrawEnableAndDeleteComponent
	ImGui::Text("");

	ComponentMeshRenderer* asMeshRenderer = static_cast<ComponentMeshRenderer*>(component);

	if (asMeshRenderer)
	{
		std::shared_ptr<ResourceMesh> meshAsShared = asMeshRenderer->GetMesh();
		std::shared_ptr<ResourceMaterial> materialAsShared = asMeshRenderer->GetMaterial();
		static char* meshPath = (char*) ("unknown");

		if (meshAsShared)
		{
			// this should not be done, see issue #240
			meshPath = (char*) (meshAsShared->GetLibraryPath().c_str());
		}
		else
		{
			meshPath = (char*) ("unknown");
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
				UID draggedMeshUID = *(UID*) payload->Data; // Double pointer to keep track correctly
				// TODO this should be Asset Path of the asset not the UID (Because new filesystem cache)
				std::shared_ptr<ResourceMesh> newMesh =
					App->GetModule<ModuleResources>()->SearchResource<ResourceMesh>(draggedMeshUID);
				// And then this should be RequestResource not SearchResource

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
			ImGui::TextColored(
				ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i ", (meshAsShared) ? meshAsShared->GetNumVertices() : 0);
			ImGui::TableNextColumn();
			ImGui::Text("Number of triangles: ");
			ImGui::TableNextColumn();
			ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f),
							   "%i ",
							   (meshAsShared) ? meshAsShared->GetNumFaces() : 0); // faces = triangles

			ImGui::EndTable();
		}
	}

	ImGui::EndChild();
	ImGui::PopStyleColor();
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
				updateMaterials = true;
				return;
			}

			ImGui::Text("");

			const char* currentShaderType = shaderTypes[materialResource->GetShaderType()].c_str();

			ImGui::Text("Shader type:");
			ImGui::SameLine();

			if (ImGui::BeginCombo("##Shader type", currentShaderType))
			{
				for (unsigned int i = 0; i < shaderTypes.size(); ++i)
				{
					const bool isSelected = materialResource->GetShaderType() == i;

					if (ImGui::Selectable(shaderTypes[i].c_str(), isSelected))
					{
						materialResource->SetShaderType(i);
						changeBatch = true;
						updateMaterials = true;
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			const char* currentType = renderModes[materialResource->IsTransparent()].c_str();

			ImGui::Text("Render Mode:");
			ImGui::SameLine();

			if (ImGui::BeginCombo("##Render mode", currentType))
			{
				for (unsigned int i = 0; i < renderModes.size(); ++i)
				{
					const bool isSelected = static_cast<int>(materialResource->IsTransparent()) == i;

					if (ImGui::Selectable(renderModes[i].c_str(), isSelected))
					{
						materialResource->SetTransparent(i);
						changeBatch = true;
						updateMaterials = true;
					}

					if (isSelected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			bool discard = asMeshRenderer->IsDiscarded();
			ImGui::Text("Discard:");
			ImGui::SameLine();
			if (ImGui::Checkbox("##Discard", &discard))
			{
				asMeshRenderer->SetDiscard(discard);
				updateMaterials = true;
			}

			ImGui::Text("Diffuse Color:");
			ImGui::SameLine();
			float4 diffuseColor = materialResource->GetDiffuseColor();
			if (ImGui::ColorEdit4("##Diffuse Color", (float*) &diffuseColor))
			{
				materialResource->SetDiffuseColor(diffuseColor);
				updateMaterials = true;
			}

			bool useDiffuseColor = asMeshRenderer->GetUseDiffuseColor();
			ImGui::Text("Use Diffuse Color:");
			ImGui::SameLine();
			if(ImGui::Checkbox("##UseDiffuseColor", &useDiffuseColor))
			{
				asMeshRenderer->SetUseDiffuseColor(useDiffuseColor);
				updateMaterials = true;
			}

			if (materialResource->GetShaderType() == 1)
			{
				ImGui::Text("Specular Color:");
				ImGui::SameLine();

				float3 specularColor = materialResource->GetSpecularColor();
				if (ImGui::ColorEdit3("##Specular Color", (float*) &specularColor))
				{
					materialResource->SetSpecularColor(specularColor);
					updateMaterials = true;
				}
			}

			ImGui::Text("");

			static UID thisUID = UniqueID::GenerateUID();

			std::string removeButtonLabel = "No Texture";

			if (materialResource)
			{
				if (materialResource->GetDiffuse() || materialResource->GetNormal() ||
					materialResource->GetMetallic() || materialResource->GetSpecular()
					|| materialResource->GetEmission())
				{
					removeButtonLabel = "Remove Textures";
				}
			}

			if (ImGui::Button(removeButtonLabel.c_str()) && materialResource)
			{
				asMeshRenderer->UnloadTextures();
				materialResource->SetDiffuse(nullptr);
				materialResource->SetNormal(nullptr);
				materialResource->SetMetallic(nullptr);
				materialResource->SetSpecular(nullptr);
				materialResource->SetEmission(nullptr);
				updateMaterials = true;
			}

			ImGui::Separator();

			ImGui::Text("Diffuse Texture");
			if (materialResource->GetDiffuse())
			{
				materialResource->GetDiffuse()->Load();
				ImGui::Image((void*) (intptr_t) materialResource->GetDiffuse()->GetGlTexture(), ImVec2(100, 100));
				if (ImGui::Button("Remove Texture Diffuse"))
				{
					materialResource->GetDiffuse()->Unload();
					materialResource->SetDiffuse(nullptr);
					updateMaterials = true;
				}
			}
			else
			{
				inputTextureDiffuse->DrawWindowContents();
			}

			ImGui::Separator();

			if (materialResource->GetShaderType() == 0)
			{
				ImGui::Text("Metallic Texture");
				if (materialResource->GetMetallic())
				{
					materialResource->GetMetallic()->Load();
					ImGui::Image((void*) (intptr_t) materialResource->GetMetallic()->GetGlTexture(), ImVec2(100, 100));

					if (ImGui::Button("Remove Texture Metallic"))
					{
						materialResource->GetMetallic()->Unload();
						materialResource->SetMetallic(nullptr);
						updateMaterials = true;
					}
				}
				else
				{
					inputTextureMetallic->DrawWindowContents();
				}
			}

			float smoothness = materialResource->GetSmoothness();
			if (ImGui::DragFloat("Smoothness", &smoothness, 0.01f, 0.0f, 1.0f))
			{
				materialResource->SetSmoothness(smoothness);
				updateMaterials = true;
			}

			if (materialResource->GetShaderType() == 0)
			{
				float metalness = materialResource->GetMetalness();
				if (ImGui::DragFloat("Metallic", &metalness, 0.01f, 0.0f, 1.0f))
				{
					materialResource->SetMetalness(metalness);
					updateMaterials = true;
				}
			}

			ImGui::Separator();

			if (materialResource->GetShaderType() == 1)
			{
				ImGui::Text("Specular Texture");

				if (materialResource->GetSpecular())
				{
					materialResource->GetSpecular()->Load();
					ImGui::Image((void*) (intptr_t) materialResource->GetSpecular()->GetGlTexture(), ImVec2(100, 100));

					if (ImGui::Button("Remove Texture Specular"))
					{
						materialResource->GetSpecular()->Unload();
						materialResource->SetSpecular(nullptr);
						updateMaterials = true;
					}
				}
				else
				{
					inputTextureSpecular->DrawWindowContents();
				}
			}

			ImGui::Text("Normal Texture");

			if (materialResource->GetNormal())
			{
				materialResource->GetNormal()->Load();
				ImGui::Image((void*) (intptr_t) materialResource->GetNormal()->GetGlTexture(), ImVec2(100, 100));

				if (ImGui::Button("Remove Texture Normal"))
				{
					materialResource->GetNormal()->Unload();
					materialResource->SetNormal(nullptr);
					updateMaterials = true;
				}
			}
			else
			{
				inputTextureNormal->DrawWindowContents();
			}

			float normalStrength = materialResource->GetNormalStrength();
			if (ImGui::DragFloat("Normal Strength", &normalStrength, 0.01f, 0.0f, std::numeric_limits<float>::max()))
			{
				materialResource->SetNormalStrength(normalStrength);
				updateMaterials = true;
			}
			ImGui::Separator();

			ImGui::Text("Emission Texture");
			if (materialResource->GetEmission())
			{
				materialResource->GetEmission()->Load();
				ImGui::Image((void*) (intptr_t) materialResource->GetEmission()->GetGlTexture(),
					ImVec2(100, 100));

				float intensityBloom = materialResource->GetIntensityBloom();
				ImGui::Text("Bloom Intensity");
				ImGui::SameLine();
				ImGui::SetNextItemWidth(60.0f);
				if (ImGui::DragFloat("##IntensityBloom", &intensityBloom, 0.05f, BLOOM_MIN_VALUE, BLOOM_MAX_VALUE, "%.2f"))
				{
					if (intensityBloom > BLOOM_MAX_VALUE)
					{
						intensityBloom = BLOOM_MAX_VALUE;
					}
					else if (intensityBloom < BLOOM_MIN_VALUE)
					{
						intensityBloom = BLOOM_MIN_VALUE;
					}
					materialResource->SetIntensityBloom(intensityBloom);
					updateMaterials = true;
				}

				if (ImGui::Button("Remove Texture Emission"))
				{
					materialResource->GetEmission()->Unload();
					materialResource->SetEmission(nullptr);
					updateMaterials = true;
				}
			}
			else
			{
				inputTextureEmission->DrawWindowContents();
			}


			ImGui::Text("");

			float2 tiling = materialResource->GetTiling();
			if (ImGui::InputFloat2("Tiling", &tiling[0], "%.1f"))
			{
				if (tiling[0] < 0.0f)
				{
					tiling[0] = 0.0f;
				}

				if (tiling[1] < 0.0f)
				{
					tiling[1] = 0.0f;
				}
				materialResource->SetTiling(tiling);
				updateMaterials = true;
			}

			float2 offset = materialResource->GetOffset();
			if (ImGui::InputFloat2("Offset", &offset[0], "%.3f"))
			{
				if (offset[0] < 0.0f)
				{
					offset[0] = 0.0f;
				}
				else if (offset[0] > 1.0f)
				{
					offset[0] = 1.0f;
				}

				if (offset[1] < 0.0f)
				{
					offset[1] = 0.0f;
				}
				else if (offset[1] > 1.0f)
				{
					offset[1] = 1.0f;
				}
				materialResource->SetOffset(offset);
				updateMaterials = true;
			}
			
			ImGui::Text("");
			ImGui::SameLine(ImGui::GetWindowWidth() - 120);

			if (ImGui::Button("Reset"))
			{
				ResetMaterialValues();
			}

			ImGui::SameLine(ImGui::GetWindowWidth() - 70);

			if (ImGui::Button("Apply"))
			{
				InitMaterialValues();
				materialResource->SetChanged(true);
				App->GetModule<ModuleResources>()->ReimportResource(materialResource->GetUID());
			}
		}

		if (changeBatch)
		{
			ChangedBatch();
		}

		if (updateMaterials)
		{
			MaterialChanged();
		}
	}
}

void WindowComponentMeshRenderer::DrawEmptyMaterial()
{
	const ComponentMeshRenderer* asMeshRenderer = static_cast<ComponentMeshRenderer*>(component);

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
	ComponentMeshRenderer* asMeshRenderer = static_cast<ComponentMeshRenderer*>(component);

	if (asMeshRenderer)
	{
		std::shared_ptr<ResourceMaterial> materialResource = asMeshRenderer->GetMaterial();

		if (materialResource)
		{
			materialCopy = std::make_shared<ResourceMaterial>(*materialResource);
			changed = false;
		}
	}
}

void WindowComponentMeshRenderer::ResetMaterialValues()
{
	ComponentMeshRenderer* asMeshRenderer = static_cast<ComponentMeshRenderer*>(component);

	if (asMeshRenderer)
	{
		std::shared_ptr<ResourceMaterial> materialResource = asMeshRenderer->GetMaterial();

		if (materialResource)
		{
			if (materialResource->IsTransparent() != materialCopy->IsTransparent() ||
				materialResource->GetShaderType() != materialCopy->GetShaderType())
			{
				changeBatch = true;
			}

			materialResource->CopyValues(*materialCopy);

			MaterialChanged();

			if (changeBatch)
			{
				ChangedBatch();
			}

			changed = false;
		}
	}
}

void WindowComponentMeshRenderer::ChangedBatch()
{
	ComponentMeshRenderer* asMeshRenderer = static_cast<ComponentMeshRenderer*>(component);

	if (asMeshRenderer)
	{
		std::vector<ComponentMeshRenderer*> componentToMove;
		componentToMove.clear();
		componentToMove = asMeshRenderer->ChangeOfBatch();
		for (ComponentMeshRenderer* component : componentToMove)
		{
			App->GetModule<ModuleRender>()->GetBatchManager()->AddComponent(component);
		}
		changeBatch = false;
	}
}

 void WindowComponentMeshRenderer::MaterialChanged()
{
	 ComponentMeshRenderer* asMeshRenderer = static_cast<ComponentMeshRenderer*>(component);

	 if (asMeshRenderer && asMeshRenderer->GetBatch())
	 {
		 asMeshRenderer->GetBatch()->SetFillMaterials(true);
	 }
	 changed = true;
 }