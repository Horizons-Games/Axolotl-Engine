#include "StdAfx.h"
#include "AnimatedTexture.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentMeshRenderer.h"

REGISTERCLASS(AnimatedTexture);

AnimatedTexture::AnimatedTexture() : Script(), movementX(0.004f), movementY(0.004f), maxOffsetX(1.0f), maxOffsetY(1.0f), 
activeTimer(false), waitTimer(0.0f), waitTimerCount(0.0f)
{
	REGISTER_FIELD(movementX, float);
	REGISTER_FIELD(movementY, float);
	REGISTER_FIELD(maxOffsetX, float);
	REGISTER_FIELD(maxOffsetY, float);
	REGISTER_FIELD(activeTimer, bool);
	REGISTER_FIELD(waitTimer, float);
}

void AnimatedTexture::Start()
{
	mesh = owner->GetComponent<ComponentMeshRenderer>();
	currentOffsetX = 0.f;
	currentOffsetY = 0.f;
}

void AnimatedTexture::Update(float deltaTime)
{
	if (activeTimer && waitTimerCount > 0.0f)
	{
		waitTimerCount -= deltaTime;
		return;
	}

	currentOffsetX += movementX;
	if (maxOffsetX < Abs(currentOffsetX))
	{
		currentOffsetX = 0.f;
	}
	currentOffsetY += movementY;
	if (maxOffsetY < Abs(currentOffsetY))
	{
		currentOffsetY = 0.f;
	} 
	mesh->SetOffset(float2(currentOffsetX, currentOffsetY));

	if (activeTimer) 
	{
		waitTimerCount = waitTimer;
	}
}