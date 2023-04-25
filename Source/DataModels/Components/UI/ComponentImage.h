#pragma once
#include "Auxiliar/Generics/Drawable.h"
#include "DataModels/Components/Component.h"

#include "Math/float4.h"
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

	float4 GetFullColor() const;

	std::shared_ptr<ResourceTexture> GetImage() const;
	float4 GetColor() const;

	void SetImage(const std::shared_ptr<ResourceTexture>& image);
	void SetColor(const float4& color);

private:
	std::shared_ptr<ResourceTexture> image;
	float4 color;
};

inline std::shared_ptr<ResourceTexture> ComponentImage::GetImage() const
{
	return image;
}

inline float4 ComponentImage::GetColor() const
{
	return color;
}

inline void ComponentImage::SetImage(const std::shared_ptr<ResourceTexture>& image)
{
	this->image = image;
}

inline void ComponentImage::SetColor(const float4& color)
{
	this->color = color;
}
