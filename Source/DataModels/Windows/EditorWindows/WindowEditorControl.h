#pragma once
#include "EditorWindow.h"

class GameObject;

class WindowEditorControl : public EditorWindow
{
public:
	WindowEditorControl();
	~WindowEditorControl();

protected:
	void DrawWindowContents() override;
};

