#include "BatchManager.h"

#include "DataModels/Batch/GeometryBatch.h"
#include "DataModels/Components/ComponentMeshRenderer.h"
#include "DataModels/Resources/ResourceMesh.h"
#include "DataModels/Resources/ResourceMaterial.h"
#include "DataModels/Batch/BatchFlags.h"

#include <assert.h>

BatchManager::BatchManager()
{
}

BatchManager::~BatchManager()
{
	geometryBatchesOpaques.clear();
	geometryBatchesTransparent.clear();
}

void BatchManager::AddComponent(ComponentMeshRenderer* newComponent)
{
	if (newComponent)
	{
		GeometryBatch* batch = CheckBatchCompatibility(newComponent);

		//adapt the batch to the good one before adding the component
		std::vector<GeometryBatch*>& geometryBatches =
			newComponent->GetMaterial()->IsTransparent()
			? geometryBatchesTransparent
			: geometryBatchesOpaques;

		if (batch)
		{
			batch->AddComponentMeshRenderer(newComponent);
		}
		else
		{
			batch = new GeometryBatch();

			batch->AddComponentMeshRenderer(newComponent);
			batch->CreateVAO();
			geometryBatches.push_back(batch);
		}
	}
}

GeometryBatch* BatchManager::CheckBatchCompatibility(const ComponentMeshRenderer* newComponent)
{
	int flags = 0;

	if (newComponent->GetMesh()->GetNormals().size() != 0)
	{
		flags |= HAS_NORMALS;
	}

	if (newComponent->GetMesh()->GetTextureCoords().size() != 0)
	{
		flags |= HAS_TEXTURE_COORDINATES;
	}

	if (newComponent->GetMesh()->GetTangents().size() != 0)
	{
		flags |= HAS_TANGENTS;
	}
	//verify if it's transparent or opaque
	std::vector<GeometryBatch*>& geometry_batches =
		newComponent->GetMaterial()->IsTransparent()
		? geometryBatchesTransparent
		: geometryBatchesOpaques;

	for (GeometryBatch* geometryBatch : geometry_batches)
	{
		if (geometryBatch->GetFlags() == flags)
		{
			return geometryBatch;
		}
	}
	return nullptr;
}

void BatchManager::DrawOpaque(GeometryBatch* batch, const std::vector<ComponentMeshRenderer*>& componentsToRender)
{
		for (GeometryBatch* geometry_batch : geometryBatchesOpaques)
		{
			DrawBatch(batch, componentsToRender);
		}
}

void BatchManager::DrawTransparent(GeometryBatch* batch, const std::vector<ComponentMeshRenderer*>& componentsToRender)
{
	for (GeometryBatch* geometry_batch : geometryBatchesTransparent)
	{
		DrawBatch(batch, componentsToRender);
	}
}

void BatchManager::DrawBatch(GeometryBatch* batch, const std::vector<ComponentMeshRenderer*>& componentsToRender)
{
	if (batch->dirtyBatch)
	{
		batch->ClearBuffer();
		batch->CreateVAO();
		batch->UpdateBatchComponents();
		batch->dirtyBatch = false;

	}
	batch->BindBatch(componentsToRender);
}
