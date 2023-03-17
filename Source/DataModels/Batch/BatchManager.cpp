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
			geometryBatches.push_back(batch);
		}
		batch->CreateVAO();
	}
}

void BatchManager::RemoveComponent(ComponentMeshRenderer* component)
{
	if (component)
	{
		GeometryBatch* batch = component->GetBatch();
		assert(batch);
		batch->RemoveComponent(component);
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

void BatchManager::DrawBatch(GeometryBatch* batch, std::vector<ComponentMeshRenderer*>& componentsToRender)
{
	batch->BindBatch(componentsToRender);
}
