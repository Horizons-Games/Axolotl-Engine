#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentRigidBody;
class UIImageDisplacementControl;
class ModuleInput;

class UITextTrigger : public Script
{

public:
	UITextTrigger();
	~UITextTrigger();

	void Start() override;
	void Update(float deltaTime) override;

	void OnCollisionEnter(ComponentRigidBody* other) override;
	void NextText();
	void TextEnd();

private:
	bool wasInside = false;
	bool dialogueDone;

	float textBoxSize;
	float textBoxCurrent;

	GameObject* currentText;

	GameObject* mission;
	GameObject* pauseManager;
	ComponentRigidBody* componentRigidBody;
	std::vector<GameObject*> textBox;

	ModuleInput* input;
	UIImageDisplacementControl* displacementControl;
};