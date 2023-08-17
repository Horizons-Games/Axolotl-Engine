#pragma once
#include "Auxiliar/Generics/Drawable.h"
#include "Component.h"

#include "GameObject/GameObject.h"

#include "Math/float3.h"

class ComponentCameraSample : public Component, public Drawable
{

public:
	ComponentCameraSample(const bool active, GameObject* owner);
	ComponentCameraSample(const ComponentCameraSample& componentCameraSample);

	~ComponentCameraSample() override;

	void Draw() const override;

	bool GetCombatCameraEnabled() const;
	void SetCombatCameraEnabled(bool enabled);

	float GetRadius() const;
	void SetRadius(float radius);

	float3 GetOffset() const;
	void SetOffset(float3 offset);

	float2 GetFocusOffset() const;
	void SetFocusOffset(float2 offset);

	bool GetFocusOffsetEnabled() const;
	void SetFocusOffsetEnabled(bool enabled);

	float3 GetFixedOffset() const;
	void SetFixedOffset(float3 offset);

	bool GetFixedOffsetEnabled() const;
	void SetFixedOffsetEnabled(bool enabled);

	float GetKpPosition() const;
	void SetKpPosition(float kp);

	bool GetKpPositionEnabled() const;
	void SetKpPositionEnabled(bool enabled);

	float GetKpRotation() const;
	void SetKpRotation(float kp);

	bool GetKpRotationEnabled() const;
	void SetKpRotationEnabled(bool enabled);

	float3 GetPosition() const;

private:
	
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	bool isCombatCameraEnabled;

	float3 position;
	float influenceRadius;
	float3 positionOffset;

	bool isSampleFixedEnabled;
	float3 positionFixed;

	bool isSampleFocusEnabled;
	float2 focusOffset;

	bool isSampleKpPositionEnabled;
	float KpPosition;

	bool isSampleKpRotationEnabled;
	float KpRotation;

};

inline bool ComponentCameraSample::GetCombatCameraEnabled() const
{
	return isCombatCameraEnabled;
}

inline void ComponentCameraSample::SetCombatCameraEnabled(bool enabled)
{
	isCombatCameraEnabled = enabled;
}

inline float ComponentCameraSample::GetRadius() const
{
	return influenceRadius;
}

inline void ComponentCameraSample::SetRadius(float radius)
{
	influenceRadius = radius;
}

inline float3 ComponentCameraSample::GetOffset() const
{
	return positionOffset;
}

inline void ComponentCameraSample::SetOffset(float3 offset)
{
	positionOffset = offset;
}

inline bool ComponentCameraSample::GetFixedOffsetEnabled() const
{
	return isSampleFixedEnabled;
}

inline void ComponentCameraSample::SetFixedOffsetEnabled(bool enabled)
{
	isSampleFixedEnabled = enabled;
}

inline bool ComponentCameraSample::GetFocusOffsetEnabled() const
{
	return isSampleFocusEnabled;
}

inline float3 ComponentCameraSample::GetFixedOffset() const
{
	return positionFixed;
}

inline void ComponentCameraSample::SetFixedOffset(float3 offset)
{
	positionFixed = offset;
}

inline void ComponentCameraSample::SetFocusOffsetEnabled(bool enabled)
{
	isSampleFocusEnabled = enabled;
}

inline float2 ComponentCameraSample::GetFocusOffset() const
{
	return focusOffset;
}

inline void ComponentCameraSample::SetFocusOffset(float2 offset)
{
	focusOffset = offset;
}

inline bool ComponentCameraSample::GetKpPositionEnabled() const
{
	return isSampleKpPositionEnabled;
}

inline void ComponentCameraSample::SetKpPositionEnabled(bool enabled)
{
	isSampleKpPositionEnabled = enabled;
}

inline float ComponentCameraSample::GetKpPosition() const
{
	return KpPosition;
}

inline void ComponentCameraSample::SetKpPosition(float kp)
{
	KpPosition = kp;
}

inline bool ComponentCameraSample::GetKpRotationEnabled() const
{
	return isSampleKpRotationEnabled;
}

inline void ComponentCameraSample::SetKpRotationEnabled(bool enabled)
{
	isSampleKpRotationEnabled = enabled;
}

inline float ComponentCameraSample::GetKpRotation() const
{
	return KpRotation;
}

inline void ComponentCameraSample::SetKpRotation(float kp)
{
	KpRotation = kp;
}

inline float3 ComponentCameraSample::GetPosition() const
{
	return position;
}