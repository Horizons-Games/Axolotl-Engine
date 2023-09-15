#include "StdAfx.h"
#include "Application.h"
#include "AnimatedLine.h"
#include "Components/ComponentLine.h"

REGISTERCLASS(AnimatedLine);


AnimatedLine::AnimatedLine() : Script(),offset(0.f),movement(0.004f)
{
}

void AnimatedLine::Start()
{
	line = owner->GetComponent<ComponentLine>();
}

void AnimatedLine::Update(float deltaTime)
{
	offset += movement;
	if (offset > 1.f) offset = 0.f;
	if (line)
	{
		line->SetTime(App->GetDeltaTime() * line->GetSpeed());
		line->SetOffset(float2(offset, 0.f));
	}
}