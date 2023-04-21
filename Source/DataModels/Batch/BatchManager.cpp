#include "BatchManager.h"

#include "DataModels/Batch/GeometryBatch.h"
#include "DataModels/Components/ComponentMeshRenderer.h"
#include "DataModels/Resources/ResourceMesh.h"
#include "DataModels/Batch/BatchFlags.h"

#include <assert.h>

BatchManager::BatchManager()
{
}

BatchManager::~BatchManager()
{
	geometryBatches.clear();
}

void BatchManager::AddComponent(ComponentMeshRenderer* newComponent)
{
	if (newComponent)
	{
		GeometryBatch* batch = CheckBatchCompatibility(newComponent);

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

	for (GeometryBatch* geometryBatch : geometryBatches)
	{
		if (geometryBatch->GetFlags() == flags)
		{
			return geometryBatch;
		}
	}
	return nullptr;
}

void BatchManager::DrawBatch(GeometryBatch* batch, const std::vector<ComponentMeshRenderer*>& componentsToRender)
{
	if (batch->dirtyBatch)
	{
		batch->ClearBuffer();
		batch->CreateVAO();
		batch->dirtyBatch = false;

	}
		batch->BindBatch(componentsToRender);
}
