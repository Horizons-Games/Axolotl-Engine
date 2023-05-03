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

void BatchManager::DrawOpaque()
{
		for (GeometryBatch* geometryBatch : geometryBatchesOpaques)
		{
			if (!geometryBatch->IsEmpty())
			{
				DrawBatch(geometryBatch);
			}
			else
			{
				erase_if(geometryBatchesOpaques, [](auto const& pi) { return pi->IsEmpty(); });
				delete geometryBatch;
			}
		}
}

void BatchManager::DrawTransparent()
{
	for (GeometryBatch* geometryBatch : geometryBatchesTransparent)
	{
		if (!geometryBatch->IsEmpty())
		{
			DrawBatch(geometryBatch);
		}
		else
		{
			erase_if(geometryBatchesOpaques, [](auto const& pi) { return pi->IsEmpty(); });
			delete geometryBatch;
		}
	}
}

void BatchManager::DrawBatch(GeometryBatch* batch)
{
	if (batch->dirtyBatch)
	{
		batch->ClearBuffer();
		batch->CreateVAO();
		batch->UpdateBatchComponents();
		batch->dirtyBatch = false;

	}
	batch->BindBatch();
}
