#ifndef _MODULE_DEBUGDRAW_H_
#define _MODULE_DEBUGDRAW_H_

#include "Module.h"

#include "Geometry/AABB.h"
#include "Geometry/OBB.h"
#include "Geometry/Frustum.h"

class DDRenderInterfaceCoreGL;
class Camera;
class ComponentTransform;

class ModuleDebugDraw : public Module
{

public:

    ModuleDebugDraw();
    ~ModuleDebugDraw() override;

    bool            Init() override;
    update_status   Update() override;
    bool            CleanUp() override;

    void            Draw(const float4x4& view, const float4x4& proj, unsigned width, unsigned height);
    void            DrawTransform(ComponentTransform* transform);
    void            DrawBoundingBox(const AABB& aabb);
    void            DrawBoundingBox(const OBB& obb);
    void            DrawFrustum(const Frustum& frustum);
    void            ShowBoundingBoxes(bool showBoundingBoxes);
    bool            IsShowingBoundingBoxes() const;

private:
    bool showBoundingBoxes;
    static DDRenderInterfaceCoreGL* implementation;
};

#endif /* _MODULE_DEBUGDRAW_H_ */