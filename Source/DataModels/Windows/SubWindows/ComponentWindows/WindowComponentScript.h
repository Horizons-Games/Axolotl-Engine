#pragma once

#include "ComponentWindow.h"

#include "FileSystem/UID.h"

class ComponentScript;
class WindowStateMachineInput;
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

	void CheckStateMachinesInput(const IScript* scriptObject);

	UID windowUID;

	std::vector<std::unique_ptr<WindowStateMachineInput>> inputStates;
	int stateMachineCount;
};
