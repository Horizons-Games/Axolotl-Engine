#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentImage;

class UICursorScript : public Script
{
public:
	UICursorScript();
	~UICursorScript() override = default;

	void Start() override;
	void Update(float deltaTime) override;

private:
	GameObject* cursor;
	ComponentImage* cursorImage;
};