#pragma once

#include "Windows/Window.h"


#include "imgui.h"
#include <ImGui/imgui_impl_sdl.h>
#include <ImGui/imgui_impl_opengl3.h>
#include <ImGui/imgui_internal.h>

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
