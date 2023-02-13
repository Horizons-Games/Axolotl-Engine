#include "WindowLoading.h"

WindowLoading::WindowLoading() : EditorWindow("Loading")
{
	flags |= ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize;
}

WindowLoading::~WindowLoading()
{
}

void WindowLoading::DrawWindowContents()
{
	ImGui::SetWindowPos(ImGui::GetWindowViewport()->GetCenter());
	ImGui::SetWindowSize(this->GetStartingSize());
	ImGui::SetWindowCollapsed(false);
	ImGui::SetWindowFocus();
	
	ENGINE_LOG("Entra");
	ImGui::TextUnformatted("Loading...");
}
