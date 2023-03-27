#pragma once
#include "Component.h"
#include "Math/float2.h"
class ComponentCanvas :  public Component
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
private:
	void RecalculateSizeAndScreenFactor();
	bool AnyChildHasCanvasRenderer(const GameObject*) const;
	bool AnyParentHasCanvas();

private:
	float2 screenReferenceSize;
	float2 size;
	float screenFactor;

};

