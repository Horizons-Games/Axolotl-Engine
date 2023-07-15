#pragma once
#include "Windows/SubWindows/ComponentWindows/ComponentWindow.h"

class ComponentTrail;
class ResourceTexture;
class WindowTrailTexture;

class WindowComponentTrail : public ComponentWindow
{
public:
	WindowComponentTrail(ComponentTrail* component);
	~WindowComponentTrail() override;

	void SetTexture(const std::shared_ptr<ResourceTexture>& texture);

protected:
	void DrawWindowContents() override;

private:
	void Init();

	constexpr static float max_intensity = 5000.0f;
	std::shared_ptr<ResourceTexture> texture;
	std::unique_ptr<WindowTrailTexture> inputTexture;
};

inline void WindowComponentTrail::SetTexture(const std::shared_ptr<ResourceTexture>& texture)
{
	this->texture = texture;
}
