#pragma once

#include "Windows/Window.h"

#include "imgui.h"

class EditorWindow : public Window
{
public:
	~EditorWindow();

	void Draw(bool& enabled) override;

	bool IsFocused() const;

protected:
	EditorWindow(const std::string& name);
	virtual void DrawWindowContents() = 0;
	virtual ImVec2 GetStartingSize() const = 0; 

	ImGuiWindowFlags flags = ImGuiWindowFlags_None;

private:
	bool focused = false;
};

inline bool EditorWindow::IsFocused() const
{
	return focused;
}
