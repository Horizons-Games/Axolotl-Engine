#pragma once
#include "Component.h"
#include <memory>

class ResourceTexture;

class ComponentImage :
	public Component
{
public:
	ComponentImage(bool active, GameObject* owner);
	~ComponentImage() override;

	void Update() override;
	void Draw() override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

private:
	std::shared_ptr<ResourceTexture> image;
	unsigned int vertexbuffer;
};

