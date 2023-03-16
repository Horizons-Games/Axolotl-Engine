#pragma once

#include <vector>

class GeometryBatch;
class ComponentMeshRenderer;

class BatchManager
{
public:
	BatchManager();
	~BatchManager();

	const std::vector<GeometryBatch*>& GetBatches() const;

	void AddComponent(ComponentMeshRenderer* newComponent);
	void RemoveComponent(ComponentMeshRenderer* component);

	void DrawBatch(GeometryBatch* geometry_batch) const;
	void DrawBatch(GeometryBatch* batch, std::vector<ComponentMeshRenderer*>& componentsToRender);

private:
	GeometryBatch* CheckBatchCompatibility(const ComponentMeshRenderer* newComponent);

	std::vector<GeometryBatch*> geometryBatches;
};

inline const std::vector<GeometryBatch*>& BatchManager::GetBatches() const
{
	return geometryBatches;
}