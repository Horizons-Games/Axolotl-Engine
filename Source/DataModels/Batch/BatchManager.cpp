#include "BatchManager.h"

#include "DataModels/Batch/GeometryBatch.h"

BatchManager::BatchManager()
{}

BatchManager::~BatchManager()
{
	geometryBatches.clear();
}

void BatchManager::AddComponent(const std::shared_ptr<ComponentMeshRenderer>& newComponent)
{
	const std::shared_ptr<GeometryBatch>& batch = CheckBatchCompatibility(newComponent);

	if (batch != nullptr)
	{
		batch->AddComponentMeshRenderer(newComponent);
	}
	else
	{
		const std::shared_ptr<GeometryBatch> newBatch = std::make_shared<GeometryBatch>();

		if (newBatch != nullptr)
		{
			newBatch->AddComponentMeshRenderer(newComponent);
			geometryBatches.push_back(newBatch);
		}
	}
}

const std::shared_ptr<GeometryBatch>& BatchManager::CheckBatchCompatibility(const std::shared_ptr<ComponentMeshRenderer>& newComponent)
{
	return nullptr;
}