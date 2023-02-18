#include "BatchManager.h"

#include "DataModels/Batch/GeometryBatch.h"
#include "DataModels/Components/ComponentMeshRenderer.h"
#include "DataModels/Resources/ResourceMesh.h"
#include "DataModels/Batch/BatchFlags.h"

BatchManager::BatchManager()
{}

BatchManager::~BatchManager()
{
	geometryBatches.clear();
}

void BatchManager::AddComponent(ComponentMeshRenderer* newComponent)
{
	if (newComponent != nullptr)
	{
		GeometryBatch* batch = CheckBatchCompatibility(newComponent);

		if (batch != nullptr)
		{
			batch->AddComponentMeshRenderer(newComponent);
		}
		else
		{
			GeometryBatch* newBatch = new GeometryBatch();

			if (newBatch != nullptr)
			{
				newBatch->AddComponentMeshRenderer(newComponent);
				geometryBatches.push_back(newBatch);
			}
		}
	}
}

GeometryBatch* BatchManager::CheckBatchCompatibility(const ComponentMeshRenderer* newComponent)
{
	int flags = 0;

	if (!newComponent->GetMesh()->GetNormals().empty())
		flags << HAS_NORMALS;

	if (!newComponent->GetMesh()->GetTextureCoords().empty())
		flags << HAS_TEXTURE_COORDINATES;

	if (!newComponent->GetMesh()->GetTangents().empty())
		flags << HAS_TANGENTS;

	for (GeometryBatch* geometryBatch : geometryBatches)
	{
		if (!(geometryBatch->GetFlags() ^ flags))
			return geometryBatch;
	}
	return nullptr;
}