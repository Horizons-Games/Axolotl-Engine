#pragma once

#include "Script.h"

class OtraPrueba : public Script
{
public:
	virtual void Init() override {
		ENGINE_LOG("OtraPrueba Init");
	}

	virtual void Start() override {

	}
	virtual void Update(float deltaTime);
};
