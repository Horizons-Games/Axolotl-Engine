#pragma once

#include "Scripting\Script.h"

class ChangeNameClass : public Script
{
public:
	virtual void Init() override;

	virtual void Start() override {
	
	}
	virtual void Update(float deltaTime);
};
