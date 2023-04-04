#pragma once

#include "ComponentWindow.h"
#include "Math/float3.h"

class ComponentMaterial;
class WindowTextureInput;
class WindowMaterialInput;
class ResourceTexture;

class WindowComponentMaterial : public ComponentWindow
{
public:
	WindowComponentMaterial(ComponentMaterial* component);
	~WindowComponentMaterial() override;

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
	std::shared_ptr<ResourceTexture> diffuseTexture;
	std::shared_ptr<ResourceTexture> metalicMap;
	std::shared_ptr<ResourceTexture> normalMap;
	float smoothness;
	float metalness;
	float normalStrength;
	bool isTransparent;
	int currentTransparentIndex;
	static const std::vector<std::string> renderModes;


	std::unique_ptr<WindowMaterialInput> inputMaterial;	   
	std::unique_ptr<WindowTextureInput> inputTextureDiffuse;
	std::unique_ptr<WindowTextureInput> inputTextureNormal;
	//std::unique_ptr<WindowTextureInput> inputTextureSpecular;
	std::unique_ptr<WindowTextureInput> inputTextureMetallic;
};

inline void WindowComponentMaterial::SetDiffuse(const std::shared_ptr<ResourceTexture>& diffuseTexture)
{
	this->diffuseTexture = diffuseTexture;
}

inline void WindowComponentMaterial::SetMetalic(const std::shared_ptr<ResourceTexture>& metalicMap)
{
	this->metalicMap = metalicMap;
}

inline void WindowComponentMaterial::SetNormal(const std::shared_ptr<ResourceTexture>& normalMap)
{
	this->normalMap = normalMap;
}
