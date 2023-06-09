#pragma once

#include "ComponentWindow.h"

#include "FileSystem/UniqueID.h"

class ComponentScript;

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

	std::string DrawStringField(std::string value, std::string name);
	float DrawFloatField(float value, std::string name);

	void OpenCreateNewScriptPopUp();
	void AddNewScriptToProject(const std::string& scriptName);

	void ReplaceSubstringsInString(std::string& stringToReplace, const std::string& from, const std::string& to);

	UID windowUID;
};
