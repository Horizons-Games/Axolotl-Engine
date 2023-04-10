#pragma once
#include "../Component.h"
#include "Math/float3.h"
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

	std::shared_ptr<ResourceTexture>& GetImage();
	float3& GetColor();
	float3 GetFullColor();
	void SetImage(const std::shared_ptr<ResourceTexture>& image);

private:
	std::shared_ptr<ResourceTexture> image;
	float3 color;
};

inline std::shared_ptr<ResourceTexture>& ComponentImage::GetImage()
{
	return image;
}

inline float3& ComponentImage::GetColor()
{
	return color;
}

inline void ComponentImage::SetImage(const std::shared_ptr<ResourceTexture>& image)
{
	this->image = image;
}

