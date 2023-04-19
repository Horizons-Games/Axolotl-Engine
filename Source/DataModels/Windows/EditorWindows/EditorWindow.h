#pragma once

#include "Windows/Window.h"

#include "imgui.h"
class EditorWindow : public Window
{
public:
	virtual ~EditorWindow() override;

	void Draw(bool& enabled) override;

	bool IsFocused() const;

protected:
	EditorWindow(const std::string& name);
	virtual void DrawWindowContents() = 0;

	ImGuiWindowFlags flags;

private:
	bool focused;
};

inline bool EditorWindow::IsFocused() const
{
	return focused;
}
