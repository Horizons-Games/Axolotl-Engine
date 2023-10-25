#include "StdAfx.h"
#include "UIApearDisapear.h"

#include "../Scripts/HealthSystem.h"

#include "ModulePlayer.h"
#include "Application.h"

#include "Components/ComponentScript.h"
#include "Components/UI/ComponentImage.h"

REGISTERCLASS(UIApearDisapear);

UIApearDisapear::UIApearDisapear() : Script(), inObjective(false), objectiveAlpha(0)
{
}

UIApearDisapear::~UIApearDisapear()
{
}

void UIApearDisapear::Start()
{
	image = GetOwner()->GetComponent<ComponentImage>();
	objectiveAlpha = image->GetColor().w;
}

void UIApearDisapear::Update(float deltaTime)
{
	if (!inObjective)
	{
		float4 color = image->GetColor();
		if (color.w > objectiveAlpha)
		{
			color.w -= 0.05f;
			image->SetColor(color);
		}
		else if(color.w < objectiveAlpha)
		{
			color.w += 0.05f;
			image->SetColor(color);
		}
		else
		{
			inObjective = true;
		}
	}
}

void UIApearDisapear::SetObjective(float newAlpha)
{
	if (newAlpha != objectiveAlpha)
	{
		objectiveAlpha = newAlpha;
		inObjective = false;
	}
}