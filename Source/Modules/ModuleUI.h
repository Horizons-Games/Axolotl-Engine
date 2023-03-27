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

	void DrawChildren(GameObject* gameObject);
	void RecalculateCanvasSizeAndScreenFactor();
};

