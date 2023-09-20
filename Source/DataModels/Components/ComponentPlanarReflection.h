#pragma once

#include "ComponentLight.h"

#include "Geometry/AABB.h"

#include "GL/glew.h"

class ComponentPlanarReflection : public ComponentLight
{
public:

	ComponentPlanarReflection(GameObject* parent);
	~ComponentPlanarReflection();

	void Draw() const override;

	void OnTransformChanged() override;

	void InitBuffer(unsigned width, unsigned height);
	void UpdateReflection(Frustum* cameraFrustum);

	void ScaleInfluenceAABB(float3& scaling);

	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

private:
	const float3 GetPosition();
	const Quat& GetRotation();

	GLuint frameBuffer;
	GLuint depth;
	GLuint reflectionTex;

	AABB influenceAABB;

	float3 planeNormal;

	float3 originScaling;
};

