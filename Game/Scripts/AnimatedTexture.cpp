#include "StdAfx.h"
#include "AnimatedTexture.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentMeshRenderer.h"

REGISTERCLASS(AnimatedTexture);


AnimatedTexture::AnimatedTexture() : Script()
{
	REGISTER_FIELD(state, StateMachine);
	REGISTER_FIELD(offset, float);
}

void AnimatedTexture::Start()
{
	mesh = owner->GetComponent<ComponentMeshRenderer>();
	offset = 0.f;
	movement = 0.004f;
}

void AnimatedTexture::Update(float deltaTime)
{
	offset += movement;
	if (offset > 1.f) offset = 0.f;
	mesh->SetOffset(float2(0.f, offset));
}