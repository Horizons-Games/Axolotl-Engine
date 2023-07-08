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
};
