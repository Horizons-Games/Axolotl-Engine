#include "StdAfx.h"
#include "UIText.h"

#include "Components/ComponentScript.h"
#include "Components/UI/ComponentSlider.h"
#include "Components/UI/ComponentImage.h"

#include "Application.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

REGISTERCLASS(UIText);

UIText::UIText() : Script(), actualNumber(0.0f), prevNumber(0.1f)
{
	REGISTER_FIELD(numbersIMG, std::vector<GameObject*>);
	REGISTER_FIELD(actualNumber, float);
}

void UIText::Init()
{
	inputPositions.push_back(owner->GetChildren()[0]->GetChildren()[0]);
	inputPositions.push_back(owner->GetChildren()[0]->GetChildren()[1]);
	inputPositions.push_back(owner->GetChildren()[0]->GetChildren()[2]);
}

void UIText::Update(float deltaTime)
{
	//FpsMetric(deltaTime); NEXT FEATURE WHEN SARA LET ME DO IT
	if (actualNumber != prevNumber)
	{
		prevNumber = actualNumber;
		AddInputVisuals(actualNumber);
	}
}

void UIText::AddInputVisuals(float numFrom)
{
		int num = static_cast<int>(numFrom);
		int digit = 0;
		int numShow[3];
		GameObject* prefab = nullptr;

		CleanInputVisuals();

		for (int a=0; a < 3; a++)
		{
			digit = num % 10;
			
			if (num !=0)
			{
				num /= 10;
				prefab = numbersIMG[digit];

				GameObject* newInput =
					App->GetModule<ModuleScene>()->GetLoadedScene()->DuplicateGameObject(prefab->GetName(), prefab, inputPositions[a]);
				newInput->Enable();
				inputVisuals.push_front(newInput);
			}
		}
}

void UIText::CleanInputVisuals()
{
	for (GameObject* input : inputVisuals)
	{
		App->GetModule<ModuleScene>()->GetLoadedScene()->DestroyGameObject(input);
	}
	inputVisuals.clear();
}

void UIText::FpsMetric(float time)
{
	currentTime = time;
	timeDiff = currentTime - previusTime;
	counter++;

	if (timeDiff >= 1.0f / 30.0f)
	{
		fps = (1.0f / timeDiff) * counter;
		ms = (timeDiff / counter) * 1000.0f;
		previusTime = currentTime;
		counter = 0.0f;
	}
	AddInputVisuals(fps);
	LOG_INFO("fps {} - -- mls {}", fps, ms);
}