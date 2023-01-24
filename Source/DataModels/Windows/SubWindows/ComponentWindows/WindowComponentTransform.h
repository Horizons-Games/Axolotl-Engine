#pragma once

#include "ComponentWindow.h"

class ComponentTransform;

class WindowComponentTransform : public ComponentWindow
{
public:
	WindowComponentTransform(const std::weak_ptr<ComponentTransform>& component);
	~WindowComponentTransform() override = default;

protected:
	void DrawWindowContents() override;

private:
	void DrawTransformTable();
	void UpdateComponentTransform();
	void UpdateLights();

	float3 currentTranslation;
	float3 currentRotation;
	float3 currentScale;
	float currentDragSpeed;
	bool translationModified;
	bool rotationModified;
	bool scaleModified;
};

