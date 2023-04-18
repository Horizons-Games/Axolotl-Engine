#pragma once
#include "DataModels/Components/Component.h"
#include "Auxiliar/Generics/Drawable.h"

#include "Math/float3.h"
#include <memory>

class ResourceTexture;
class ResourceMesh;

class ComponentImage : public Component, public Drawable
{
public:
	ComponentImage(bool active, GameObject* owner);
	~ComponentImage() override;

	void Draw() const override;

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	std::shared_ptr<ResourceTexture> GetImage() const;
	float3 GetColor() const;
	float3 GetFullColor() const;
	void SetImage(const std::shared_ptr<ResourceTexture>& image);

	void LoadVBO();
	void CreateVAO();

private:
	std::shared_ptr<ResourceTexture> image;
	float3 color;

	unsigned vbo;
	unsigned vao;

};

inline std::shared_ptr<ResourceTexture> ComponentImage::GetImage() const
{
	return image;
}

inline float3 ComponentImage::GetColor() const
{
	return color;
}

inline void ComponentImage::SetImage(const std::shared_ptr<ResourceTexture>& image)
{
	this->image = image;
}

