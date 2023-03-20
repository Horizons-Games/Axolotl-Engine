#include "WindowStyle.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include <ImGui/imgui_internal.h>



WindowStyle::WindowStyle() : SubWindow("Style")
{
}

WindowStyle::~WindowStyle()
{
}

void WindowStyle::DrawWindowContents()
{
	ImGui::ShowStyleEditor();
}

void WindowStyle::MainDockConfig()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGuiID dockSpaceId = ImGui::GetID("DockSpace");


	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	
	ImGuiWindowFlags dockSpaceWindowFlags = 0;
	dockSpaceWindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
	dockSpaceWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", nullptr, dockSpaceWindowFlags);
	ImGui::PopStyleVar(3);
	ImGui::DockSpace(dockSpaceId);
	if (!App->fileSystem->Exists("imgui.ini"))
		{
		ImGui::DockBuilderRemoveNode(dockSpaceId); // clear any previous layout
		ImGui::DockBuilderAddNode(dockSpaceId, dockSpaceWindowFlags | ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dockSpaceId, viewport->Size);

		ImGuiID dockIdUp = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Up, 0.08f, nullptr, &dockSpaceId);
		ImGuiID dockIdRight = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.27f, nullptr, &dockSpaceId);
		ImGuiID dockIdDown = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.32f, nullptr, &dockSpaceId);
		ImGuiID dockIdLeft = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Left, 0.22f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Console", dockIdDown);
		ImGui::DockBuilderDockWindow("File Browser", dockIdDown);
		ImGui::DockBuilderDockWindow("Configuration", dockIdRight);
		ImGui::DockBuilderDockWindow("Inspector", dockIdRight);
		ImGui::DockBuilderDockWindow("Editor Control", dockIdUp);
		ImGui::DockBuilderDockWindow("Hierarchy", dockIdLeft);
		ImGui::DockBuilderDockWindow("Scene", dockSpaceId);
		ImGui::DockBuilderFinish(dockSpaceId);
	}
	
}

void WindowStyle::ResetDockConfig()
{
	App->fileSystem->Delete("imgui.ini");
}
