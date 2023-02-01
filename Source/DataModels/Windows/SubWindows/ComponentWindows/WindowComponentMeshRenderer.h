#pragma once

#include "ComponentWindow.h"

class ComponentMeshRenderer;
class WindowMeshInput;

class WindowComponentMeshRenderer : public ComponentWindow
{
public:
	WindowComponentMeshRenderer(const std::weak_ptr<ComponentMeshRenderer>& component);
	~WindowComponentMeshRenderer() override = default;

protected:
	void DrawWindowContents() override;

private:
	std::unique_ptr<WindowMeshInput> inputMesh;
};

