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

	void TextureCompressions();

	void InitTextureImportOptions();

	void InitTextureLoadOptions();

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

	std::shared_ptr<ResourceMaterial> materialCopy;

	std::unique_ptr<WindowMeshInput> inputMesh;
	std::unique_ptr<WindowMaterialInput> inputMaterial;
	std::unique_ptr<WindowTextureInput> inputTextureDiffuse;
	std::unique_ptr<WindowTextureInput> inputTextureNormal;
	std::unique_ptr<WindowTextureInput> inputTextureMetallic;
	std::unique_ptr<WindowTextureInput> inputTextureSpecular;
	std::unique_ptr<WindowTextureInput> inputTextureEmission;
	std::weak_ptr<Resource> resource;

	ComponentMeshRenderer* oldComponent;

	bool reset;
	bool newMaterial;
	bool changeBatch;

	bool changed;

	bool flag;
	bool compressionDiffuse  = false ;
	bool compressionNormal   = false ;
	bool compressionMetallic = false ;
	bool compressionSpecular = false ;
	bool compressionEmission = false ;

	bool flipVertical;
	bool flipHorizontal;

	bool mipMap;
	int min;
	int mag;
	int wrapS;
	int wrapT;
	int compressionLevel;
};
