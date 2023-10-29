#include "StdAfx.h"
#include "TurnDisable.h"

#include "Application.h"

REGISTERCLASS(TurnDisable);

TurnDisable::TurnDisable() : Script(), counter(5.f)
{
}

TurnDisable::~TurnDisable()
{
}

void TurnDisable::Update(float deltaTime)
{
	counter -= App->GetDeltaTime();
	if (counter <= 0.f)
	{
		Disable();
		owner->Disable();
	}
}
