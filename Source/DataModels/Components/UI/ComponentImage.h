#pragma once
#include "../Component.h"
#include "Math/float4.h"
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

	float4 GetFullColor();

	std::shared_ptr<ResourceTexture>& GetImage();
	float4 GetColor() const;

	void SetImage(const std::shared_ptr<ResourceTexture>& image);
	void SetColor(const float4& color);

private:
	std::shared_ptr<ResourceTexture> image;
	float4 color;
};

inline std::shared_ptr<ResourceTexture>& ComponentImage::GetImage()
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

