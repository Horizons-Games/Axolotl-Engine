#ifndef _MODULE_DEBUGDRAW_H_
#define _MODULE_DEBUGDRAW_H_

#pragma warning(disable : 26495)

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

	bool Init();
	update_status Update();
	bool CleanUp();

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

#endif /* _MODULE_DEBUGDRAW_H_ */