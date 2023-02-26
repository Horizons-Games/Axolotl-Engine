#pragma once
#include "EditorWindow.h"

class GameObject;

class WindowHierarchy : public EditorWindow
{
public:
	WindowHierarchy();
	~WindowHierarchy();

protected:
	void DrawWindowContents() override;

private:
	void DrawRecursiveHierarchy(GameObject* gameObject);
};
