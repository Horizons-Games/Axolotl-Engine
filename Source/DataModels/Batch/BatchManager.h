#pragma once

#include <vector>

#ifndef ENGINE
#include "FileSystem/ModuleResources.h"
#endif // !ENGINE

class GeometryBatch;
class ComponentMeshRenderer;

class BatchManager
{
public:
	BatchManager();
	~BatchManager();

	void AddComponent(ComponentMeshRenderer* newComponent);

	void DrawBatch(GeometryBatch* batch, const std::vector<ComponentMeshRenderer*>& componentsToRender);

	void CleanBatches();

private:
	GeometryBatch* CheckBatchCompatibility(const ComponentMeshRenderer* newComponent);

	std::vector<GeometryBatch*> geometryBatches;
};

inline void BatchManager::CleanBatches()
{
#ifndef ENGINE
	App->resources->CleanResourceBin();
#endif // !ENGINE
	geometryBatches.clear();
}