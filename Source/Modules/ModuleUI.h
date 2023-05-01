#pragma once
#include "Module.h"


class GameObject;

class ModuleUI : public Module
{
public:
	ModuleUI();
	~ModuleUI();

	bool Init() override;

	update_status Update() override;

	update_status PostUpdate() override;

	void RecalculateCanvasSizeAndScreenFactor();
	void LoadVBO();
	void CreateVAO();
	unsigned int GetQuadVAO() const;

private:
	unsigned int quadVBO;
	unsigned int quadVAO;
};

inline bool ModuleUI::Start()
{
	return true;
}

inline unsigned int ModuleUI::GetQuadVAO() const
{
	return quadVAO;
}


