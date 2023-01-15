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
	void Display() override;

	void CalculateBoundingBoxes();
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void Encapsule(const vec* Vertices, unsigned numVertices);

	std::string GetTypeName() override;

	const AABB& GetLocalABB();
	const AABB& GetEncapsuledAABB();
	const OBB& GetObjectOBB();
	const bool isDrawBoundingBoxes();

	void setDrawBoundingBoxes( bool newDraw );

private:
	AABB localAABB;
	AABB encapsuledAABB;
	OBB objectOBB;
	bool drawBoundingBoxes;
};

inline void ComponentBoundingBoxes::Encapsule(const vec* Vertices, unsigned numVertices)
{
	localAABB.Enclose(Vertices, numVertices);
}

inline std::string ComponentBoundingBoxes::GetTypeName()
{
	return COMPONENT_BOUNDING;
}

inline const AABB& ComponentBoundingBoxes::GetLocalABB()
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