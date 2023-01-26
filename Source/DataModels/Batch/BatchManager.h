#pragma once

#include <vector>
#include <memory>

class GeometryBatch;
class ComponentMeshRenderer;

class BatchManager
{
public:
	BatchManager();
	~BatchManager();

	void AddComponent(const std::shared_ptr<ComponentMeshRenderer>& newComponent);
private:
	const std::shared_ptr<GeometryBatch>& CheckBatchCompatibility(const std::shared_ptr<ComponentMeshRenderer>& newComponent);

	std::vector<std::shared_ptr<GeometryBatch>> geometryBatches;
};