#pragma once
#include "IObject.h"
struct IUpdateable : public IObject
{
	virtual void Update(float deltaTime) = 0;
};