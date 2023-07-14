#pragma once

#include "ComponentWindow.h"

#include "FileSystem/UID.h"

class ComponentScript;
class WindowStateMachineInput;
class StateMachine;
class IScript;

class WindowComponentScript : public ComponentWindow
{
public:
	WindowComponentScript(ComponentScript* component);
	~WindowComponentScript() override;

protected:
	void DrawWindowContents() override;
	int current_item = 0;

private:
	void ChangeScript(ComponentScript* newScript, const char* selectedScript);

	void OpenCreateNewScriptPopUp();
	void AddNewScriptToProject(const std::string& scriptName);

	void ReplaceSubstringsInString(std::string& stringToReplace, const std::string& from, const std::string& to);

	void UpdateStateMachinesInputVector(const IScript* scriptObject);

	bool IsDoubleClicked();

	void StateMachineField(StateMachine* value, const std::string& name, const std::string& nameInstance);

	float secondsSinceLastClick = 0;
	UID windowUID;

	std::vector<std::unique_ptr<WindowStateMachineInput>> inputStates;
	int stateMachineCount = -1;
};
