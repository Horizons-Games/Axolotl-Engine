#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class UIOptionsMenu;

class UILoadSavedSettings : public Script
{
public:
	UILoadSavedSettings();
	~UILoadSavedSettings() override = default;

	void Start() override;

private:

	UIOptionsMenu* optionsMenu;

};