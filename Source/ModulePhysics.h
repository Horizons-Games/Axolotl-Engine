#pragma once

#include "Module.h"

class ModulePhysics : public Module
{
public:
	ModulePhysics();
	~ModulePhysics() override;

	bool Init() override;
	update_status Update();
	bool CleanUp();

	
private:

};