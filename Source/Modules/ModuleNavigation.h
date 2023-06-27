#pragma once
#include "Module.h"

class ModuleNavigation : public Module
{
public:
	ModuleNavigation();
	~ModuleNavigation() override;

	bool Init() override;
	bool Start() override;

	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;

	bool CleanUp() override;
};
