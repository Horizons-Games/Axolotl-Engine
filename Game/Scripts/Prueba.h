#pragma once

#include "Scripting\Script.h"

class Prueba : public Script
{
public:
	virtual void Init() {

	}
	virtual void Start() {

	}
	virtual void Update(float deltaTime)
	{
		owner->SetName("prueba");
	}
};
