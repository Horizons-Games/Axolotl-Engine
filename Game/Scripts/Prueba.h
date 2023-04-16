#pragma once
#include "EngineLog.h"
#include "Scripting\Script.h"
#include "Scripting\SystemTable.h"
#include "ISimpleSerializer.h"

#include "GameObject/GameObject.h"
#include "Application.h"

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
REGISTERCLASS(Prueba);