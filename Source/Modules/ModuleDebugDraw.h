#ifndef _MODULE_DEBUGDRAW_H_
#define _MODULE_DEBUGDRAW_H_

#include "Module.h"

#include "Geometry/AABB.h"
#include "Geometry/Frustum.h"
#include "Geometry/OBB.h"

class DDRenderInterfaceCoreGL;
class Camera;
class ComponentTransform;

class ModuleDebugDraw : public Module
{
public:
	ModuleDebugDraw();
	~ModuleDebugDraw() override;

	bool Init() override;
	UpdateStatus Update() override;
	bool CleanUp() override;

	void Draw(const float4x4& view, const float4x4& proj, unsigned width, unsigned height);
	void DrawTransform(ComponentTransform* transform);
	void DrawBoundingBox(const AABB& aabb);
	void DrawBoundingBox(const OBB& obb);
	void DrawFrustum(const Frustum& frustum);
	void ShowBoundingBoxes(bool showBoundingBoxes);
	bool IsShowingBoundingBoxes() const;
	void ShowGrid(bool showGrid);
	bool IsShowingGrid() const;
	void ShowRigidBody(bool showRigidBody);
	bool IsShowingRigidBody() const;

private:
	bool showBoundingBoxes;
	bool showRigidBody;
	bool showGrid;
	static DDRenderInterfaceCoreGL* implementation;
};

inline void ModuleDebugDraw::ShowBoundingBoxes(bool showBoundingBoxes)
{
	this->showBoundingBoxes = showBoundingBoxes;
}

inline bool ModuleDebugDraw::IsShowingBoundingBoxes() const
{
	return showBoundingBoxes;
}

inline void ModuleDebugDraw::ShowGrid(bool showGrid)
{
	this->showGrid = showGrid;
}

inline bool ModuleDebugDraw::IsShowingGrid() const
{
	return showGrid;
}

inline void ModuleDebugDraw::ShowRigidBody(bool showRigidBody)
{
	this->showRigidBody = showRigidBody;
}

inline bool ModuleDebugDraw::IsShowingRigidBody() const
{
	return showRigidBody;
}

#endif /* _MODULE_DEBUGDRAW_H_ */
