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

	void RecalculateCanvasSizeAndScreenFactor();
	void LoadVBO();
	void CreateVAO();
	unsigned int GetQuadVAO() const;

private:
	unsigned int quadVBO;
	unsigned int quadVAO;
};

inline bool ModuleUI::Init()
{
	return true;
}

inline bool ModuleUI::Start()
{
	return true;
}

inline unsigned int ModuleUI::GetQuadVAO() const
{
	return quadVAO;
}


