#pragma once
#include "EngineLog.h"
#include "Scripting\Script.h"
#include "Scripting\SystemTable.h"
#include "ISimpleSerializer.h"

#include "GameObject/GameObject.h"
#include "Application.h"

class ChangeNameClass : public Script
{
public:
	virtual void Init() override {
		ENGINE_LOG("ChangeNameClass Init");
	}

	virtual void Start() override {
	
	}
	virtual void Update(float deltaTime)
	{
		owner->SetName("pruebfdwga");
	}
};
REGISTERCLASS(ChangeNameClass);