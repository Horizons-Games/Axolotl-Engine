#pragma once

#include "Windows/Window.h"

#include "imgui.h"

class SubWindow : public Window
{
public:
	virtual ~SubWindow() override;

	void Draw() override;

protected:
	SubWindow(const std::string& name);
	virtual void DrawWindowContents() = 0;

	ImGuiTreeNodeFlags flags;

private:
	static bool defaultEnabled;
};

