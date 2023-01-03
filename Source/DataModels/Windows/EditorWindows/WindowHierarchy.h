#pragma once
#include "EditorWindow.h"
#include "Windows/SubWindows/SubWindow.h"
#include "Windows/UID.h"

#include <vector>
#include <memory>

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

	std::vector<std::unique_ptr<SubWindow> > collapsingSubWindows;

	void DrawRecursiveHierarchy(GameObject* gameObject);

};

inline ImVec2 WindowHierarchy::GetStartingSize() const
{
	return ImVec2(900, 250);
}
