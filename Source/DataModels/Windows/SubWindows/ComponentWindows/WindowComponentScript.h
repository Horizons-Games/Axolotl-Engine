#pragma once

#include "ComponentWindow.h"

#include "FileSystem/UID.h"
#include "GameObject/GameObject.h"

class ComponentScript;
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

	bool IsDoubleClicked();

	float secondsSinceLastClick = 0;
	UID windowUID;
	int currentItem = 0;
};
