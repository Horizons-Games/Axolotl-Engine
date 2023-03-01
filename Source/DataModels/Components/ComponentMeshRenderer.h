#pragma once

#include "Components/Component.h"
#include "Geometry/AABB.h"
#include "Geometry/OBB.h"

#include <memory>

#define COMPONENT_MESHRENDERED "MeshRendered"

class ResourceMaterial;
class ResourceMesh;
class Json;
class WindowMeshInput;

class ComponentMeshRenderer : public Component
{
public:
	ComponentMeshRenderer(const bool active, GameObject* owner);
	~ComponentMeshRenderer() override;

	void CalculateBoundingBoxes();
	void Update() override;

	void Draw() override;
	void DrawHighlight();

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void SetMesh(const std::shared_ptr<ResourceMesh>& newMesh);

	std::shared_ptr<ResourceMesh> GetMesh() const;

	void Encapsule(const vec* Vertices, unsigned numVertices);

	const AABB& GetLocalAABB();
	const AABB& GetEncapsuledAABB();
	const OBB& GetObjectOBB();
	const bool isDrawBoundingBoxes() const;

	void setDrawBoundingBoxes(bool newDraw);

private:
	bool IsMeshLoaded();

	std::shared_ptr<ResourceMesh> mesh;

	AABB* localAABB;
	AABB* encapsuledAABB;
	OBB* objectOBB;
	bool drawBoundingBoxes;

	WindowMeshInput* inputMesh;

	friend class WindowComponentBoundingBoxes;
};

inline std::shared_ptr<ResourceMesh> ComponentMeshRenderer::GetMesh() const
{
	return mesh;
}

inline const AABB& ComponentMeshRenderer::GetLocalAABB()
{
	CalculateBoundingBoxes();
	return *localAABB;
}

inline const AABB& ComponentMeshRenderer::GetEncapsuledAABB()
{
	CalculateBoundingBoxes();
	return *encapsuledAABB;
}

inline const OBB& ComponentMeshRenderer::GetObjectOBB()
{
	CalculateBoundingBoxes();
	return *objectOBB;
}

inline const bool ComponentMeshRenderer::isDrawBoundingBoxes() const
{
	return drawBoundingBoxes;
}

inline bool ComponentMeshRenderer::IsMeshLoaded()
{
	return mesh != nullptr;
}