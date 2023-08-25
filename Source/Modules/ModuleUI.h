#pragma once
#include "Math/float2.h"
#include "Module.h"

class GameObject;

class ModuleUI : public Module
{
public:
	ModuleUI();
	~ModuleUI();

	bool Init() override;

	UpdateStatus Update() override;

	UpdateStatus PostUpdate() override;

	void RecalculateCanvasSizeAndScreenFactor();
	void LoadVBO();
	void CreateVAO();
	unsigned int GetQuadVAO() const;

	float2 GetCursorPosition() const;
	void SetCursorPosition(float2 position);

	float GetCursorRotation() const;
	void SetCursorRotation(float rotation);

private:
	void DetectInteractionWithGameObject(const GameObject* gameObject,
										 float2 cursorPosition,
										 bool leftClicked,
										 bool disabledHierarchy);
	void Draw2DGameObject(const GameObject* gameObject);

	unsigned int quadVBO;
	unsigned int quadVAO;

	// Only a float2 because in 2D contexts we don't need the Z position
	float2 cursorPosition;

	// Only a float because in 2D contexts we only use the Z axis rotation
	float cursorRotation;
};

inline unsigned int ModuleUI::GetQuadVAO() const
{
	return quadVAO;
}

inline void ModuleUI::SetCursorPosition(float2 position)
{
	cursorPosition = position;
}

inline float2 ModuleUI::GetCursorPosition() const
{
	return cursorPosition;
}

inline void ModuleUI::SetCursorRotation(float rotation)
{
	cursorRotation = rotation;
}

inline float ModuleUI::GetCursorRotation() const
{
	return cursorRotation;
}