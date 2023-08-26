#pragma once

#ifndef ENGINE
	#include "FileSystem/ModuleResources.h"

	#include "GameObject/GameObject.h"
#endif // !ENGINE

class GeometryBatch;
class ComponentMeshRenderer;
class GameObject;

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
		HAS_TRANSPARENCY = 0X00000040,
		IS_A_CHARACTER = 0X00000080
	};

public:
	BatchManager();
	~BatchManager();

	void FillCharactersBacthes();
	void SwapBatchParentAndChildren(GameObject* go);

	void AddComponent(ComponentMeshRenderer* newComponent);

	void DrawMeshes(std::vector<GameObject*>& objects, const float3& pos);
	void DrawOpaque(bool selected);
	void DrawTransparent(bool selected);
	void DrawBatch(GeometryBatch* batch, bool selected);
	void DrawBatch(GeometryBatch* batch, std::vector<GameObject*>& meshes);

	void SetDirtybatches();

	void CleanBatches();

private:
	void SearchAndSwapBatchCharacter(GameObject* parent);
	GeometryBatch* CheckBatchCompatibility(const ComponentMeshRenderer* newComponent, int& flags);
	bool IsACharacter(const ComponentMeshRenderer* newComponent);

	std::vector<GeometryBatch*> geometryBatchesOpaques;
	std::vector<GeometryBatch*> geometryBatchesTransparent;
};