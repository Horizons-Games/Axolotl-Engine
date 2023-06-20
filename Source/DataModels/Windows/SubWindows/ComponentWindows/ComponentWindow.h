#pragma once

#include "Windows/SubWindows/SubWindow.h"

#include <memory>

class Component;

class ComponentWindow : public SubWindow
{
public:
	virtual ~ComponentWindow() override;

	static std::unique_ptr<ComponentWindow> CreateWindowForComponent(Component* component);

protected:
	ComponentWindow(const std::string& name, Component* component);
	void DrawEnableAndDeleteComponent();

	Component* component;

private:
	void DrawEnableComponent();
	void DrawDeleteComponent();

	// for the enable and delete labels
	std::string windowUUID;
};
