#pragma once

#include "ComponentWindow.h"

#include "FileSystem/UID.h"
#include "GameObject/GameObject.h"

class ComponentScript;
class WindowStateMachineInput;
class StateMachine;
class IScript;
namespace math
{
class float3;
}

class WindowComponentScript : public ComponentWindow
{
public:
	WindowComponentScript(ComponentScript* component);
	~WindowComponentScript() override;

protected:
	void DrawWindowContents() override;

private:
	void ChangeScript(ComponentScript* newScript, const char* selectedScript);

	std::string DrawStringField(std::string& value, const std::string& name);
	bool DrawBoolField(bool& value, const std::string& name);
	float DrawFloatField(float& value, const std::string& name);
	math::float3 DrawFloat3Field(math::float3& value, const std::string& name);
	GameObject* DrawGameObjectField(GameObject* value, const std::string& name);

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
	int currentItem = 0;
};
