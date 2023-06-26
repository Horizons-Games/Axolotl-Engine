#pragma once
#include "ComponentWindow.h"

class ComponentSlider;
class GameObject;

class WindowComponentSlider : public ComponentWindow
{
public:
	WindowComponentSlider(ComponentSlider* component);
	~WindowComponentSlider() override;

protected:
	void DrawWindowContents() override;
	bool DrawGameObject(GameObject*& go, std::string label);
};
