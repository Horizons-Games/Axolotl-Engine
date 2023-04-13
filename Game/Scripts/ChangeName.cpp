#pragma once
#include "EngineLog.h"
#include "SystemTable.h"
#include "Script.h"

#include "GameObject/GameObject.h"

class ChangeNameClass : public Script
{
public:
	virtual void Update(float deltaTime)
	{
		GameObject* o = PerModuleInterface::g_pSystemTable->owner;
		o->SetName("Terciary");
	}
};
REGISTERCLASS(ChangeNameClass);