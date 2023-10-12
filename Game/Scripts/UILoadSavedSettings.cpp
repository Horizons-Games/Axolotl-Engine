#include "StdAfx.h"
#include "UILoadSavedSettings.h"

#include "Components/ComponentScript.h"
#include "UIOptionsMenu.h"


REGISTERCLASS(UILoadSavedSettings);

UILoadSavedSettings::UILoadSavedSettings() : Script(), optionsMenu(nullptr)
{
	REGISTER_FIELD(optionsMenu, UIOptionsMenu*);
	REGISTER_FIELD(applyOptionsOnLoad, bool);
}

void UILoadSavedSettings::Start()
{
	Assert(optionsMenu != nullptr, "Options Menu is not set");
	optionsMenu->SetApplyChangesOnLoad(applyOptionsOnLoad);
	optionsMenu->Initialize();
}
