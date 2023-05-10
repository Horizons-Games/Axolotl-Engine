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

	void DrawOpaque(bool selected);
	void DrawTransparent(bool selected);
	void DrawBatch(GeometryBatch* batch, bool selected);

	void CleanBatches();

private:
	GeometryBatch* CheckBatchCompatibility(const ComponentMeshRenderer* newComponent, int& flags);

	std::vector<GeometryBatch*> geometryBatchesOpaques;
	std::vector<GeometryBatch*> geometryBatchesTransparent;
};

inline void BatchManager::CleanBatches()
{
#ifndef ENGINE
	App->resources->CleanResourceBin();
#endif // !ENGINE
	geometryBatchesOpaques.clear();
	geometryBatchesTransparent.clear();
}