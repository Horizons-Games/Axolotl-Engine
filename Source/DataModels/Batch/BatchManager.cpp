#include "StdAfx.h"

#include "BatchManager.h"

#include "Application.h"

#include "ModuleScene.h"
#include "Scene/Scene.h"

#include "DataModels/Components/ComponentMeshRenderer.h"
#include "DataModels/Resources/ResourceMaterial.h"
#include "DataModels/Resources/ResourceMesh.h"

#include "GeometryBatch.h"

BatchManager::BatchManager()
{
}

BatchManager::~BatchManager()
{
	CleanBatches();
}

void BatchManager::FillCharactersBacthes()
{
	SearchAndSwapBatchCharacter(App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot());
}

void BatchManager::SwapBatchParentAndChildren(GameObject* go)
{
	ComponentMeshRenderer* component = go->GetComponentInternal<ComponentMeshRenderer>();
	if (component)
	{
		component->GetBatch()->DeleteComponent(component);
		AddComponent(component);
	}
	for (auto child : go->GetChildren())
	{
		SwapBatchParentAndChildren(child);
	}
}

void BatchManager::AddComponent(ComponentMeshRenderer* newComponent)
{
	if (newComponent)
	{
		int flags = 0;

		GeometryBatch* batch = CheckBatchCompatibility(newComponent, flags);

		if (batch)
		{
			batch->AddComponentMeshRenderer(newComponent);
		}
		else
		{
			batch = new GeometryBatch(flags);

			std::vector<GeometryBatch*>& geometryBatches =
				newComponent->GetMaterial() && newComponent->GetMaterial()->IsTransparent() ? 
				    geometryBatchesTransparent : geometryBatchesOpaques;

			batch->AddComponentMeshRenderer(newComponent);
			geometryBatches.push_back(batch);
		}
	}
}

void BatchManager::SearchAndSwapBatchCharacter(GameObject* parent)
{
	if (parent->GetTag() == "Player" || parent->GetTag() == "Enemy")
	{
		SwapBatchParentAndChildren(parent);
		return;
	}
	for (auto child : parent->GetChildren())
	{
		SearchAndSwapBatchCharacter(child);
	}
}

GeometryBatch* BatchManager::CheckBatchCompatibility(const ComponentMeshRenderer* newComponent, int& flags)
{
	std::shared_ptr<ResourceMesh> mesh = newComponent->GetMesh();
	std::shared_ptr<ResourceMaterial> material = newComponent->GetMaterial();
	if (IsACharacter(newComponent))
	{
		flags |= IS_A_CHARACTER;
	}

	if (mesh)
	{
		if (mesh->GetNormals().size() != 0)
		{
			flags |= HAS_NORMALS;
		}

		if (mesh->GetTextureCoords().size() != 0)
		{
			flags |= HAS_TEXTURE_COORDINATES;
		}

		if (mesh->GetTangents().size() != 0)
		{
			flags |= HAS_TANGENTS;
		}
	}

	if (material)
	{
		if (material->GetShaderType() == 0)
		{
			flags |= HAS_METALLIC;
		}
		else if (material->GetShaderType() == 1)
		{
			flags |= HAS_SPECULAR;
		}
		if (material->IsTransparent())
		{
			flags |= HAS_TRANSPARENCY;
		}
		else
		{
			flags |= HAS_OPAQUE;
		}
	}
	else
	{
		flags |= HAS_METALLIC;
		flags |= HAS_OPAQUE;
	}

	//verify if it's transparent or opaque
	std::vector<GeometryBatch*>& geometry_batches = 
		material && newComponent->GetMaterial()->IsTransparent() ? geometryBatchesTransparent : geometryBatchesOpaques;

	for (GeometryBatch* geometryBatch : geometry_batches)
	{
		if (geometryBatch->GetFlags() == flags)
		{
			return geometryBatch;
		}
	}
	return nullptr;
}

bool BatchManager::IsACharacter(const ComponentMeshRenderer* newComponent)
{
	GameObject* go = newComponent->GetOwner();
	GameObject* root = App->GetModule<ModuleScene>()->GetLoadedScene()->GetRoot();
	while (go != root)
	{
		if (go->GetTag() == "Player" || go->GetTag() == "Enemy")
		{
			return true;
		}
		go = go->GetParent();
	}
	return false;
}

void BatchManager::DrawOpaque(bool selected)
{
	for (GeometryBatch* geometryBatch : geometryBatchesOpaques)
	{
		if (!geometryBatch->IsEmpty())
		{
			geometryBatch->SortByDistanceCloseToFar();
			DrawBatch(geometryBatch, selected);
		}
		else
		{
			erase_if(geometryBatchesOpaques, [](auto const& gb) { return gb->IsEmpty(); });
			delete geometryBatch;
		}
	}
}

void BatchManager::DrawTransparent(bool selected)
{
	for (GeometryBatch* geometryBatch : geometryBatchesTransparent)
	{
		if (!geometryBatch->IsEmpty())
		{
			geometryBatch->SortByDistanceFarToClose();
			DrawBatch(geometryBatch, selected);
		}
		else
		{
			erase_if(geometryBatchesTransparent, [](auto const& gb) { return gb->IsEmpty(); });
			delete geometryBatch;
		}
	}
}

void BatchManager::DrawBatch(GeometryBatch* batch, bool selected)
{
	if (batch->IsDirty())
	{
		batch->ClearBuffer();
		batch->CreateVAO();
		batch->UpdateBatchComponents();
		batch->SetDirty(false);

	}
	batch->BindBatch(selected);
}

void BatchManager::SetDirtybatches()
{
	for (GeometryBatch* batch : geometryBatchesOpaques)
	{
		batch->SetDirty(true);
	}
	for (GeometryBatch* batch : geometryBatchesTransparent)
	{
		batch->SetDirty(true);
	}
}

void BatchManager::CleanBatches()
{
#ifndef ENGINE
	App->GetModule<ModuleResources>()->CleanResourceBin();
#endif // !ENGINE

	for (GeometryBatch* batch : geometryBatchesOpaques)
	{
		delete batch;
	}
	geometryBatchesOpaques.clear();

	for (GeometryBatch* batch : geometryBatchesTransparent)
	{
		delete batch;
	}
	geometryBatchesTransparent.clear();
}