#pragma once

#include "Module.h"

class ModulePhysics
{
public:
	ModulePhysics();
	~ModulePhysics();

	bool Init();
	update_status Update();
	bool CleanUp();

	
private:

};