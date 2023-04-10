#pragma once
#include "Module.h"


class GameObject;

class ModuleUI : public Module
{
public:
	ModuleUI();
	~ModuleUI();

	bool Init();
	bool Start();

	update_status Update();

	update_status PostUpdate();

	void DrawChildren(GameObject* gameObject);
	void RecalculateCanvasSizeAndScreenFactor();
	void LoadVBO();
	void CreateVAO();
	const unsigned int GetQuadVAO() const;

private:
	unsigned int quadVBO;
	unsigned int quadVAO;
};

inline const unsigned int ModuleUI::GetQuadVAO() const
{
	return quadVAO;
}


