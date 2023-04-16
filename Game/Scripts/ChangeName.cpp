#include "ChangeName.h"

void ChangeNameClass::Init()
{
	ENGINE_LOG("TEST");
}

void ChangeNameClass::Update(float deltaTime)
{
	owner->SetName("aa!");
}

REGISTERCLASS(ChangeNameClass);