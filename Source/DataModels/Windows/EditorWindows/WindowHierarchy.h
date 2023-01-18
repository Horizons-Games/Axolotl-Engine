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

	ImVec2 GetStartingSize() const override;


private:
	void DrawRecursiveHierarchy(const std::shared_ptr<GameObject>& gameObject);
};

inline ImVec2 WindowHierarchy::GetStartingSize() const
{
	return ImVec2(900, 250);
}
