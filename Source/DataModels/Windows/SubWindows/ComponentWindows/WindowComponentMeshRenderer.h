#pragma once

#include "ComponentWindow.h"
#include "Math/float4.h"

class ComponentMeshRenderer;
class WindowMeshInput;
class WindowTextureInput;
class WindowMaterialInput;
class ResourceTexture;

class WindowComponentMeshRenderer : public ComponentWindow
{
public:
	WindowComponentMeshRenderer(ComponentMeshRenderer* component);
	~WindowComponentMeshRenderer() override;

	void SetDiffuse(const std::shared_ptr<ResourceTexture>& diffuseTexture);
	void SetMetalic(const std::shared_ptr<ResourceTexture>& metalicMap);
	void SetNormal(const std::shared_ptr<ResourceTexture>& normalMap);

protected:
	void DrawWindowContents() override;

private:
	void DrawSetMaterial();
	void DrawEmptyMaterial();
	void InitMaterialValues();

	float4 colorDiffuse;
	float4 oldColorDiffuse;
	std::shared_ptr<ResourceTexture> diffuseTexture;
	std::shared_ptr<ResourceTexture> metalicMap;
	std::shared_ptr<ResourceTexture> normalMap;
	float smoothness;
	float metalness;
	float normalStrength;
	bool isTransparent;
	int currentTransparentIndex;
	static const std::vector<std::string> renderModes;

	bool reset = false;

	std::unique_ptr<WindowMeshInput> inputMesh;
	std::unique_ptr<WindowMaterialInput> inputMaterial;
	std::unique_ptr<WindowTextureInput> inputTextureDiffuse;
	std::unique_ptr<WindowTextureInput> inputTextureNormal;
	// std::unique_ptr<WindowTextureInput> inputTextureSpecular;
	std::unique_ptr<WindowTextureInput> inputTextureMetallic;
};

inline void WindowComponentMeshRenderer::SetDiffuse(const std::shared_ptr<ResourceTexture>& diffuseTexture)
{
	this->diffuseTexture = diffuseTexture;
}

inline void WindowComponentMeshRenderer::SetMetalic(const std::shared_ptr<ResourceTexture>& metalicMap)
{
	this->metalicMap = metalicMap;
}

inline void WindowComponentMeshRenderer::SetNormal(const std::shared_ptr<ResourceTexture>& normalMap)
{
	this->normalMap = normalMap;
}