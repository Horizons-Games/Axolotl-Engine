#pragma once

#include "Windows/Window.h"

#include "imgui.h"

class EditorWindow : public Window
{
public:
	virtual ~EditorWindow() override;

	void Draw() override;

	bool IsFocused() const;	
	void Start();
	void CleanUp();

protected:
	EditorWindow(const std::string& name);
	virtual void DrawWindowContents() = 0;
	virtual ImVec2 GetStartingSize() const = 0; 

	ImGuiWindowFlags flags;
	

private:
	bool focused;
	void LoadMeta(const std::string name, bool& enable);
	void UpdateMeta(bool enabled, const std::string name);
};

inline bool EditorWindow::IsFocused() const
{
	return focused;
}
