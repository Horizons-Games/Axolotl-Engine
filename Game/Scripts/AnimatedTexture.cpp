#include "StdAfx.h"
#include "AnimatedTexture.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentMeshRenderer.h"

REGISTERCLASS(AnimatedTexture);

AnimatedTexture::AnimatedTexture() : Script(), movementX(0.004f), movementY(0.004f), maxOffsetX(1.0f), maxOffsetY(1.0f)
{
	REGISTER_FIELD(movementX, float);
	REGISTER_FIELD(movementY, float);
	REGISTER_FIELD(maxOffsetX, float);
	REGISTER_FIELD(maxOffsetY, float);
}

void AnimatedTexture::Start()
{
	mesh = owner->GetComponent<ComponentMeshRenderer>();
	currentOffsetX = 0.f;
	currentOffsetY = 0.f;
}

void AnimatedTexture::Update(float deltaTime)
{
	currentOffsetX += movementX;
	if (maxOffsetX < currentOffsetX)
	{
		currentOffsetX = 0.f;
	}
	currentOffsetY += movementY;
	if (maxOffsetY < currentOffsetY)
	{
		currentOffsetY = 0.f;
	} 
	mesh->SetOffset(float2(currentOffsetX, currentOffsetY));
}