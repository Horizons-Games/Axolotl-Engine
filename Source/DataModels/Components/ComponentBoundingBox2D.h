#pragma once
#include "Component.h"

#include "Geometry/AABB2D.h"

class ComponentBoundingBox2D :
    public Component
{

	ComponentBoundingBox2D(const bool active, GameObject* owner);
	~ComponentBoundingBox2D() override;

	void Init() override;
	void Update() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void SetLocalBoundingBox(const AABB2D& boundingBox);
	void CalculateWorldBoundingBox(bool force = false);
	const AABB2D& GetWorldAABB() const;

private:
	bool drawOutline;

	AABB2D localAABB;
	AABB2D worldAABB;
};

