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
	float GetRenderPercentage() const;

	void SetImage(const std::shared_ptr<ResourceTexture>& image);
	void SetColor(const float4& color);
	void SetRenderPercentage(float renderPercentage);

private:
	std::shared_ptr<ResourceTexture> image;
	float4 color;
	float renderPercentage;
};

inline std::shared_ptr<ResourceTexture> ComponentImage::GetImage() const
{
	return image;
}

inline float4 ComponentImage::GetColor() const
{
	return color;
}

inline float ComponentImage::GetRenderPercentage() const
{
	return renderPercentage;
}

inline void ComponentImage::SetImage(const std::shared_ptr<ResourceTexture>& image)
{
	this->image = image;
}

inline void ComponentImage::SetColor(const float4& color)
{
	this->color = color;
}

inline void ComponentImage::SetRenderPercentage(float renderPercentage)
{
	this->renderPercentage = renderPercentage;
}
