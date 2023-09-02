#pragma once

#include "Windows/EditorWindows/EditorWindow.h"

class WindowNavMeshInput;

class WindowNavigation : public EditorWindow
{
public:
	WindowNavigation();
	~WindowNavigation() override;

protected:
	void DrawWindowContents() override;
	void DrawNavCreation();

	std::unique_ptr<WindowNavMeshInput> inputNavMesh;
	std::string nameNewNavMesh;
};
