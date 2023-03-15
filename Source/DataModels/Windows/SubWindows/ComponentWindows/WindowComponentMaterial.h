#pragma once

#include "ComponentWindow.h"

class ComponentMaterial;
class WindowTextureInput;
class WindowMaterialInput;

class WindowComponentMaterial : public ComponentWindow
{
public:
	WindowComponentMaterial(ComponentMaterial* component);
	~WindowComponentMaterial() override;

protected:
	void DrawWindowContents() override;

private:
	void DrawSetMaterial();
	void DrawEmptyMaterial();

	std::unique_ptr<WindowMaterialInput> inputMaterial;	   
	std::unique_ptr<WindowTextureInput> inputTextureDiffuse;
	std::unique_ptr<WindowTextureInput> inputTextureNormal;
	//std::unique_ptr<WindowTextureInput> inputTextureSpecular;
	std::unique_ptr<WindowTextureInput> inputTextureMetallic;
};

