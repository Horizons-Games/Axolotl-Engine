#pragma once
#include "../Component.h"
#include "Math/float2.h"
class ComponentCanvas : public Component
{
public:
	ComponentCanvas(bool active, GameObject* owner);
	~ComponentCanvas() override;

	void Update() override;
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	void SetScreenReferenceSize(float2 screenReferenceSize_);

	float2 GetScreenReferenceSize() const;
	float2 GetSize();
	float GetScreenFactor();
	void RecalculateSizeAndScreenFactor();

private:
	float2 screenReferenceSize;
	float2 size;
	float screenFactor;
};

inline void ComponentCanvas::SetScreenReferenceSize(float2 screenReferenceSize_)
{
	screenReferenceSize = screenReferenceSize_;
}

inline float2 ComponentCanvas::GetScreenReferenceSize() const
{
	return screenReferenceSize;
}

inline float2 ComponentCanvas::GetSize()
{
	return size;
}

inline float ComponentCanvas::GetScreenFactor()
{
	return screenFactor;
}
