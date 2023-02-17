#include "BatchManager.h"

#include "DataModels/Batch/GeometryBatch.h"
#include "DataModels/Components/ComponentMeshRenderer.h"

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
	return nullptr;
}