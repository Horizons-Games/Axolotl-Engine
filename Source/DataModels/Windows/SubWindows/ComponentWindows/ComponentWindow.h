#pragma once

#include "Windows/SubWindows/SubWindow.h"

#include "FileSystem/UniqueID.h"

class Component;

class ComponentWindow : public SubWindow
{
public:
	virtual ~ComponentWindow() = default;

	static std::unique_ptr<ComponentWindow> CreateWindowForComponent(const std::weak_ptr<Component>& component);

protected:
	ComponentWindow(const std::string& name, const std::weak_ptr<Component>& component);
	void DrawEnableAndDeleteComponent();

	std::weak_ptr<Component> component;

private:
	void DrawEnableComponent();
	void DrawDeleteComponent();

	//for the enable and delete labels
	std::string windowUUID;
};

