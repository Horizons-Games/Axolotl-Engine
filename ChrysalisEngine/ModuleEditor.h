#pragma once
#include "Module.h"

class ModuleEditor : public Module
{
public:
	ModuleEditor();
	~ModuleEditor();

	bool Init() override;
	bool Start() override;
	bool CleanUp() override;

	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;

	void Resized();

private:
	std::vector<std::string> lines;

	bool windowResized = false;
};

