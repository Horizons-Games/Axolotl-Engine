#include "StdAfx.h"
#include "AnimatedTexture.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentMeshRenderer.h"

REGISTERCLASS(AnimatedTexture);


AnimatedTexture::AnimatedTexture() : Script()
{
	REGISTER_FIELD(object, GameObject*);
}

void AnimatedTexture::Start()
{
	mesh = owner->GetComponent<ComponentMeshRenderer>();
	offset = 0.f;
}

void AnimatedTexture::Update(float deltaTime)
{
	offset += 1.f;
	if (offset > 512.f) offset = 0.f;
	mesh->SetOffset(float2(0.f, offset));
}