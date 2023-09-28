#pragma once
#include "Windows/SubWindows/ComponentWindows/ComponentWindow.h"

class ComponentTrail;
class ImGradientMark;
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

	std::shared_ptr<ResourceTexture> texture;
	std::unique_ptr<WindowTrailTexture> inputTexture;

	ImGradientMark* draggingMark;
	ImGradientMark* selectedMark;
};

inline void WindowComponentTrail::SetTexture(const std::shared_ptr<ResourceTexture>& texture)
{
	this->texture = texture;
}
