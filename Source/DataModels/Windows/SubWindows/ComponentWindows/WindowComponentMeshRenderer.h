#pragma once

#include "ComponentWindow.h"

class ComponentMeshRenderer;
class WindowMeshInput;

class WindowComponentMeshRenderer : public ComponentWindow
{
public:
	WindowComponentMeshRenderer(ComponentMeshRenderer* component);
	~WindowComponentMeshRenderer() override;

protected:
	void DrawWindowContents() override;

private:
	std::unique_ptr<WindowMeshInput> inputMesh;
	//this should not be needed
	char* meshPath;
	bool hadComponent;
};

