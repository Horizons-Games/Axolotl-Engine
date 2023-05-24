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
	update_status Update() override;
	bool CleanUp() override;

	void Draw(const float4x4& view, const float4x4& proj, unsigned width, unsigned height);
	void DrawTransform(ComponentTransform* transform);
	void DrawBoundingBox(const AABB& aabb);
	void DrawBoundingBox(const OBB& obb);
	void DrawFrustum(const Frustum& frustum);
	void ShowBoundingBoxes(bool showBoundingBoxes);
	bool IsShowingBoundingBoxes() const;

private:
	bool showBoundingBoxes;
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

#endif /* _MODULE_DEBUGDRAW_H_ */
