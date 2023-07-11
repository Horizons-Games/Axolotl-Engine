#pragma once
#include "ComponentWindow.h"

class ComponentLine;
class ImGradient;
class ResourceTexture;
class WindowLineTexture;

class WindowComponentLine : public ComponentWindow
{
public:
	WindowComponentLine(ComponentLine* component);
	~WindowComponentLine() override;

	void SetTexture(const std::shared_ptr<ResourceTexture>& texture);
	void InitValues();

protected:
	void DrawWindowContents() override;

private:

	float2 tiling;
	float2 offset;

	std::unique_ptr<WindowLineTexture> inputTexture;
	std::shared_ptr<ResourceTexture> lineTexture;

};


inline void WindowComponentLine::SetTexture(const std::shared_ptr<ResourceTexture>& texture)
{
	this->lineTexture = texture;
}
