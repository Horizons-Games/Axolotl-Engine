#include "StdAfx.h"
#include "UIText.h"

#include "Components/ComponentScript.h"
#include "Components/UI/ComponentSlider.h"
#include "Components/UI/ComponentImage.h"

#include "Application.h"
#include "ModuleScene.h"
#include "Scene/Scene.h"

REGISTERCLASS(UIText);

UIText::UIText() : Script(), actualNumber(0.0f), prevNumber(0.0f)
{
	REGISTER_FIELD(numbersImg, std::vector<GameObject*>);
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
	//FpsMetric(deltaTime);
	if (actualNumber != prevNumber)
	{
		prevNumber = actualNumber;
		AddInputVisuals(actualNumber);
	}
}

void UIText::AddInputVisuals(float numFrom)
{
		int number = static_cast<int>(numFrom);
		int digit = 0;
		int maxNumberToShow = static_cast<int>(inputPositions.size());
		GameObject* prefab = nullptr;

		CleanInputVisuals();
		
		for (int currentNumberToShow=0; currentNumberToShow < maxNumberToShow; ++currentNumberToShow)
		{
			digit = number % 10;
			
			if (number !=0)
			{
				number /= 10;
				prefab = numbersImg[digit];

				GameObject* newInput =
					App->GetModule<ModuleScene>()->GetLoadedScene()->
					DuplicateGameObject(prefab->GetName(), prefab, inputPositions[currentNumberToShow]);
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
	timeDiff = currentTime - previousTime;
	counter++;

	if (timeDiff >= 1.0f / 30.0f)
	{
		fps = (1.0f / timeDiff) * counter;
		ms = (timeDiff / counter) * 1000.0f;
		previousTime = currentTime;
		counter = 0;
	}
	AddInputVisuals(fps);
	LOG_INFO("fps {} - -- mls {}", fps, ms);
}