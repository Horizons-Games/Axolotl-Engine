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

private:
	void DetectInteractionWithGameObject(const GameObject* gameObject,
										 float2 cursorPosition,
										 bool leftClicked,
										 bool disabledHierarchy);
	void Draw2DGameObject(const GameObject* gameObject);

	unsigned int quadVBO;
	unsigned int quadVAO;
};

inline unsigned int ModuleUI::GetQuadVAO() const
{
	return quadVAO;
}