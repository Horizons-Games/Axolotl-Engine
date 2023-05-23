#pragma once
#include "ComponentWindow.h"

#include "Math/float2.h"
#include "Math/float3.h"

class ComponentTransform2D;

class WindowComponentTransform2D : public ComponentWindow
{
public:
	WindowComponentTransform2D(ComponentTransform2D* component);
	~WindowComponentTransform2D() override;

protected:
	void DrawTransformTable();
	void DrawWindowContents() override;

private:
	void UpdateComponentTransform();
	float3 currentTranslation;
	float3 currentRotation;
	float3 currentScale;
	float2 currentSize;
	float currentDragSpeed;
	bool translationModified;
	bool rotationModified;
	bool scaleModified;
	bool sizeModified;
};
