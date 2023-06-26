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
	enum BatchFlags
	{
		HAS_NORMALS = 0x00000001,
		HAS_TEXTURE_COORDINATES = 0x00000002,
		HAS_TANGENTS = 0x00000004,
		HAS_METALLIC = 0X00000008,
		HAS_SPECULAR = 0X00000010,
		HAS_OPAQUE = 0X00000020,
		HAS_TRANSPARENCY = 0X00000040
	};

public:
	BatchManager();
	~BatchManager();

	void AddComponent(ComponentMeshRenderer* newComponent);

	void DrawOpaque(bool selected);
	void DrawTransparent(bool selected);
	void DrawBatch(GeometryBatch* batch, bool selected);

	void SetDirtybatches();

	void CleanBatches();

private:
	GeometryBatch* CheckBatchCompatibility(const ComponentMeshRenderer* newComponent, int& flags);

	std::vector<GeometryBatch*> geometryBatchesOpaques;
	std::vector<GeometryBatch*> geometryBatchesTransparent;
};