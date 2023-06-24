#pragma once
#include "Module.h"

class ModuleNavigation : public Module
{
public:
	ModuleNavigation();
	~ModuleNavigation() override;

	bool Init() override;
	bool Start() override;

	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;

	bool CleanUp() override;
};
