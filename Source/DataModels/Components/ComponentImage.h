#pragma once
#include "Component.h"
#include <memory>

class ResourceTexture;
class ResourceMesh;

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

	std::shared_ptr<ResourceTexture> GetImage() const;
	void SetImage(const std::shared_ptr<ResourceTexture>& image);

	void LoadVBO();

	void CreateVAO();

private:
	std::shared_ptr<ResourceTexture> image;
	unsigned vbo;
	unsigned vao;

};

inline std::shared_ptr<ResourceTexture> ComponentImage::GetImage() const
{
	return image;
}

inline void ComponentImage::SetImage(const std::shared_ptr<ResourceTexture>& image)
{
	this->image = image;
}

