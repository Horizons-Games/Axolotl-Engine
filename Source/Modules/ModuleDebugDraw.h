#ifndef _MODULE_DEBUGDRAW_H_
#define _MODULE_DEBUGDRAW_H_

#include "Module.h"

#include "Math/float4x4.h"
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

    bool            Init();
    update_status   Update();
    bool            CleanUp();

    void            Draw(const float4x4& view, const float4x4& proj, unsigned width, unsigned height);
    void            DrawTransform(ComponentTransform* transform);
    void            DrawBoundingBox(const AABB& aabb);
    void            DrawBoundingBox(const OBB& obb);
    void            DrawFrustum(const Frustum& frustum);
    void            ShowQuadtree(bool showQuadtree);
    bool            IsShowingQuadtree() const;

private:
    bool showQuadtree;
    static DDRenderInterfaceCoreGL* implementation;
};

#endif /* _MODULE_DEBUGDRAW_H_ */