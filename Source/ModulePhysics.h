#pragma once

#include "Module.h"

class ModulePhysics : public Module
{
public:
	ModulePhysics();
	~ModulePhysics() override;

	bool Init() override;	
private:

};