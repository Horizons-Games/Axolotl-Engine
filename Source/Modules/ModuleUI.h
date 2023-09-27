#pragma once
#include "Math/float2.h"
#include "Module.h"

#include "FileSystem/UID.h"

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

	void SetUpButtons();

	void ClearButtons();

	std::size_t GetCurrentButtonIndex() const;

private:
	void DetectInteractionWithGameObject(const GameObject* gameObject,
										 float2 cursorPosition,
										 bool leftClicked,
										 bool disabledHierarchy);
	void Draw2DGameObject(const GameObject* gameObject);

	unsigned int quadVBO;
	unsigned int quadVAO;

	float lastButtonChange = 0.0f;

	std::vector<UID> sortedButtonsIds;
	std::size_t currentButtonIndex;
};

inline unsigned int ModuleUI::GetQuadVAO() const
{
	return quadVAO;
}

inline void ModuleUI::ClearButtons()
{
	sortedButtonsIds.clear();
}

inline std::size_t ModuleUI::GetCurrentButtonIndex() const
{
	return currentButtonIndex;
}
