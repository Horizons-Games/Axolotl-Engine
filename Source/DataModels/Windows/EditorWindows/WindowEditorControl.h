#pragma once
#include "EditorWindow.h"

class GameObject;

class WindowEditorControl : public EditorWindow
{
public:
	WindowEditorControl();
	~WindowEditorControl() override;

protected:
	void DrawWindowContents() override;

	ImVec2 GetStartingSize() const override;
};

inline ImVec2 WindowEditorControl::GetStartingSize() const
{
	return ImVec2(900, 250);
}
