#pragma once
#include "Component.h"

#include "Geometry/AABB.h"
#include "Geometry/OBB.h"

#define COMPONENT_BOUNDING "Bounding"

class Json;

class ComponentBoundingBoxes : public Component
{
public:
	ComponentBoundingBoxes(const bool active, GameObject* owner);

	void Update() override;

	void CalculateBoundingBoxes();
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void Encapsule(const vec* Vertices, unsigned numVertices);

	const AABB& GetLocalAABB();
	const AABB& GetEncapsuledAABB();
	const OBB& GetObjectOBB();
	const bool isDrawBoundingBoxes();

	void setDrawBoundingBoxes( bool newDraw );

private:
	AABB localAABB;
	AABB encapsuledAABB;
	OBB objectOBB;
	bool drawBoundingBoxes;

	friend class WindowComponentBoundingBoxes;
};

inline void ComponentBoundingBoxes::Encapsule(const vec* Vertices, unsigned numVertices)
{
	localAABB = localAABB.MinimalEnclosingAABB(Vertices, numVertices);
}

inline const AABB& ComponentBoundingBoxes::GetLocalAABB()
{
	CalculateBoundingBoxes();
	return localAABB;
}

inline const AABB& ComponentBoundingBoxes::GetEncapsuledAABB()
{
	CalculateBoundingBoxes();
	return encapsuledAABB;
}

inline const OBB& ComponentBoundingBoxes::GetObjectOBB()
{
	CalculateBoundingBoxes();
	return objectOBB;
}

inline void ComponentBoundingBoxes::Update()
{
}

inline const bool ComponentBoundingBoxes::isDrawBoundingBoxes()
{
	return drawBoundingBoxes;
}

inline void ComponentBoundingBoxes::setDrawBoundingBoxes(bool newDraw)
{
	drawBoundingBoxes = newDraw;
}