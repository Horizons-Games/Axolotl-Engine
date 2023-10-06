#include "StdAfx.h"
#include "UILoadSavedSettings.h"

#include "Components/ComponentScript.h"
#include "UIOptionsMenu.h"


REGISTERCLASS(UILoadSavedSettings);

UILoadSavedSettings::UILoadSavedSettings() : Script(), optionsMenu(nullptr)
{
	REGISTER_FIELD(optionsMenu, GameObject*);
}

void UILoadSavedSettings::Start()
{
	if (optionsMenu != nullptr)
	{
		UIOptionsMenu* optionMenuScript = optionsMenu->GetComponent<UIOptionsMenu>();
		
		optionMenuScript->SetLoadFromMainMenu(true);
		optionMenuScript->Init();
	}
	else
	{
		LOG_INFO("OptionsMenu register field its empty");
	}
}

void UILoadSavedSettings::Update(float deltaTime)
{

}
