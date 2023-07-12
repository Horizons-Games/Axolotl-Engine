#pragma once
#include "Windows/SubWindows/ComponentWindows/ComponentWindow.h"

class ComponentTrail;

class WindowComponentTrail : public ComponentWindow
{
public:
	WindowComponentTrail(ComponentTrail* component);
	~WindowComponentTrail() override;

protected:
	void DrawWindowContents() override;

private:
	constexpr static float max_intensity = 5000.0f;
};
