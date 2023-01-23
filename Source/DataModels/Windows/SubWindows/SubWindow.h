#pragma once

#include "Windows/Window.h"

#include "imgui.h"

class SubWindow : public Window
{
public:
	~SubWindow();

	void Draw(bool& enabled = defaultEnabled) override;

protected:
	SubWindow(const std::string& name);
	virtual void DrawWindowContents() = 0;

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_None;

private:
	static bool defaultEnabled;
};

