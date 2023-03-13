#pragma once
#include "EditorWindow.h"

class GameObject;

class WindowHierarchy : public EditorWindow
{
public:
	WindowHierarchy();
	~WindowHierarchy() override;

protected:
	void DrawWindowContents() override;

private:
	void DrawRecursiveHierarchy(GameObject* gameObject, bool isRoot);
	void DrawPopupMenu(GameObject* gameObject);

	bool stopDrawing;
};
