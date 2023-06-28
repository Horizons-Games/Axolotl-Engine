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

	float GetRadius() const;
	void SetRadius(float radius);

	float3 GetOffset() const;
	void SetOffset(float3 offset);

	float2 GetFocusOffset() const;
	void SetFocusOffset(float2 offset);

	bool GetFocusOffsetEnabled() const;
	void SetFocusOffsetEnabled(bool enabled);

	float3 GetPosition() const;

private:
	
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	float3 position;
	float influenceRadius;
	float3 positionOffset;

	bool isSampleFocusEnabled;
	float2 focusOffset;
};

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

inline bool ComponentCameraSample::GetFocusOffsetEnabled() const
{
	return isSampleFocusEnabled;
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

inline float3 ComponentCameraSample::GetPosition() const
{
	return position;
}