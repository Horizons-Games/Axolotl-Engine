#pragma once
#include "Component.h"
#include "Auxiliar/Generics/Drawable.h"

#include "Math/float4x4.h"

#include "Geometry/AABB.h"
#include "Geometry/OBB.h"
#include <DataModels/Windows/SubWindows/ComponentWindows/WindowComponentTransform.h>

class Json;
class ComponentLight;

class ComponentTransform : public Component, public Drawable
{
public:
	ComponentTransform(const bool active, GameObject* owner);
	ComponentTransform(const ComponentTransform& componentTransform);
	~ComponentTransform() override;

	void Draw() const override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	const float3& GetPosition() const;
	const float3& GetGlobalPosition() const;
	const float4x4& GetRotation() const;
	const float3& GetRotationXYZ() const;
	const float4x4& GetGlobalRotation() const;
	const float3& GetScale() const;
	const float3& GetLocalForward() const;
	const float3& GetGlobalForward() const;
	const float3& GetGlobalUp() const;
	const float3& GetGlobalRight() const;
	const float3& GetGlobalScale() const;

	const float4x4& GetLocalMatrix() const;
	const float4x4& GetGlobalMatrix() const;

	const AABB& GetLocalAABB();
	const AABB& GetEncapsuledAABB();
	const OBB& GetObjectOBB();
	bool IsDrawBoundingBoxes() const;
	bool IsUniformScale() const;

	void SetPosition(const float3& position);
	void SetRotation(const float3& rotation);
	void SetRotation(const float4x4& rotation);
	void SetScale(const float3& scale);
	void SetUniformScale(const float3& scale, Axis modifiedScaleAxis);

	void SetDrawBoundingBoxes(bool newDraw);

	void CalculateMatrices();
	void UpdateTransformMatrices();

	void CalculateLightTransformed(const ComponentLight* lightComponent,
								   bool translationModified,
								   bool rotationModified);
	
	void CalculateBoundingBoxes();
	void Encapsule(const vec* vertices, unsigned numVertices);

private:
	float3 pos;
	float4x4 rot;
	float3 sca;

	float3 globalPos;
	float4x4 globalRot;
	float3 globalSca;

	float3 rotXYZ;

	float4x4 localMatrix;
	float4x4 globalMatrix;

	AABB localAABB;
	AABB encapsuledAABB;
	OBB objectOBB;
	bool drawBoundingBoxes;
	bool uniformScale;
};

inline const float3& ComponentTransform::GetPosition() const
{
	return pos;
}

inline const float3& ComponentTransform::GetGlobalPosition() const
{
	return globalPos;
}

inline const float4x4& ComponentTransform::GetRotation() const 
{
	return rot;
}

inline const float3& ComponentTransform::GetRotationXYZ() const
{
	return rotXYZ;
}

inline const float4x4& ComponentTransform::GetGlobalRotation() const
{
	return globalRot;
}

inline const float3& ComponentTransform::GetScale() const
{
	return sca;
}

inline const float3& ComponentTransform::GetGlobalScale() const
{
	return globalSca;
}

inline const float3& ComponentTransform::GetLocalForward() const
{
	return localMatrix.WorldZ();
}

inline const float3& ComponentTransform::GetGlobalForward() const
{
	return globalMatrix.WorldZ();
}

inline const float3& ComponentTransform::GetGlobalUp() const
{
	return globalMatrix.WorldY();
}

inline const float3& ComponentTransform::GetGlobalRight() const
{
	return globalMatrix.WorldX();
}

inline const float4x4& ComponentTransform::GetLocalMatrix() const
{
	return localMatrix;
}

inline const float4x4& ComponentTransform::GetGlobalMatrix() const
{
	return globalMatrix;
}

inline const AABB& ComponentTransform::GetLocalAABB()
{
	CalculateBoundingBoxes();
	return localAABB;
}

inline const AABB& ComponentTransform::GetEncapsuledAABB()
{
	CalculateBoundingBoxes();
	return encapsuledAABB;
}

inline const OBB& ComponentTransform::GetObjectOBB()
{
	CalculateBoundingBoxes();
	return objectOBB;
}

inline bool ComponentTransform::IsUniformScale() const
{
	return uniformScale;
}

inline bool ComponentTransform::IsDrawBoundingBoxes() const
{
	return drawBoundingBoxes;
}

inline void ComponentTransform::SetPosition(const float3& position)
{
	pos = position;
}

inline void ComponentTransform::SetRotation(const float3& rotation)
{
	rotXYZ = rotation;
	rot = float4x4::FromEulerXYZ(DegToRad(rotation.x), DegToRad(rotation.y), DegToRad(rotation.z));
}

inline void ComponentTransform::SetRotation(const float4x4& rotation)
{
	rot = rotation;
	rotXYZ = RadToDeg(rotation.ToEulerXYZ());
}

inline void ComponentTransform::SetScale(const float3& scale)
{
	sca.x = std::max(scale.x, 0.0001f);
	sca.y = std::max(scale.y, 0.0001f);
	sca.z = std::max(scale.z, 0.0001f);
}

inline void ComponentTransform::SetUniformScale(const float3& scale, Axis modifiedScaleAxis)
{
	if (modifiedScaleAxis == Axis::X)
	{
		sca.y = scale.y * scale.x / sca.x;
		sca.z = scale.z * scale.x / sca.x;
		sca.x = scale.x;
	}
	else if (modifiedScaleAxis == Axis::Y)
	{
		sca.z = scale.z * scale.y / sca.y;
		sca.x = scale.x * scale.y / sca.y;
		sca.y = scale.y;
	}
	else {
		sca.x = scale.x * scale.z / sca.z;
		sca.y = scale.y * scale.z / sca.z;
		sca.z = scale.z;
	}

	if (sca.x <= 0 || sca.y <= 0 || sca.z <= 0) {
		sca.x = 0.0001f;
		sca.y = 0.0001f;
		sca.z = 0.0001f;
	}
}

inline void ComponentTransform::SetDrawBoundingBoxes(bool newDraw)
{
	drawBoundingBoxes = newDraw;
}
