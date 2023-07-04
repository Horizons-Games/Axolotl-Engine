#pragma once

#include "ComponentWindow.h"
#include "Math/float4.h"

class ComponentMeshRenderer;
class WindowMeshInput;
class WindowTextureInput;
class WindowMaterialInput;
class ResourceTexture;
class ResourceMaterial;

class WindowComponentMeshRenderer : public ComponentWindow
{
public:
	WindowComponentMeshRenderer(ComponentMeshRenderer* component);
	~WindowComponentMeshRenderer() override;

	std::shared_ptr<ResourceMaterial> GetMaterial() const;
	void SetMaterial(const std::shared_ptr<ResourceMaterial>& material);
	void MaterialChanged();

protected:
	void DrawWindowContents() override;

private:
	void DrawSetMaterial();
	void DrawEmptyMaterial();
	void InitMaterialValues();
	void ResetMaterialValues();
	void ChangedBatch();

	static const std::vector<std::string> shaderTypes;
	static const std::vector<std::string> renderModes;

	std::shared_ptr<ResourceMaterial> material;

	std::unique_ptr<WindowMeshInput> inputMesh;
	std::unique_ptr<WindowMaterialInput> inputMaterial;
	std::unique_ptr<WindowTextureInput> inputTextureDiffuse;
	std::unique_ptr<WindowTextureInput> inputTextureNormal;
	std::unique_ptr<WindowTextureInput> inputTextureMetallic;
	std::unique_ptr<WindowTextureInput> inputTextureSpecular;
	std::unique_ptr<WindowTextureInput> inputTextureEmission;

	ComponentMeshRenderer* oldComponent;

	bool reset;
	bool newMaterial;
	bool changeBatch;

	bool changed;
};

inline std::shared_ptr<ResourceMaterial> WindowComponentMeshRenderer::GetMaterial() const
{
	return material;
}

inline void WindowComponentMeshRenderer::SetMaterial(const std::shared_ptr<ResourceMaterial>& material)
{
	this->material = material;
	newMaterial = true;
}
