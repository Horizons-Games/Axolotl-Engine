#include "StdAfx.h"
#include "Application.h"
#include "AnimatedTexture.h"
#include "GameObject/GameObject.h"
#include "Components/Component.h"
#include "Components/ComponentMeshRenderer.h"
#include "Components/ComponentLine.h"

REGISTERCLASS(AnimatedTexture);


AnimatedTexture::AnimatedTexture() : Script()
{
}

void AnimatedTexture::Start()
{
	mesh = owner->GetComponent<ComponentMeshRenderer>();
	line = owner->GetComponent<ComponentLine>();
	offset = 0.f;
	movement = 0.004f;
}

void AnimatedTexture::Update(float deltaTime)
{
	offset += movement;
	if (offset > 1.f) offset = 0.f;
	if (mesh)
	{
		mesh->SetOffset(float2(0.f, offset));
	}
	if (line)
	{
		line->SetTime(App->GetDeltaTime() * line->GetSpeed());
		line->SetOffset(float2(offset, 0.f));
	}
}