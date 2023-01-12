#include "Application.h"
#include "Globals.h"

#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleEngineCamera.h"
#include "ModuleTexture.h"

#include "Windows/WindowMainMenu.h"
#include "Windows/EditorWindows/WindowConsole.h"
#include "Windows/EditorWindows/WindowScene.h"
#include "Windows/EditorWindows/WindowConfiguration.h"
#include "Windows/EditorWindows/WindowInspector.h"
#include "Windows/EditorWindows/WindowHierarchy.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_sdl.h>
#include <ImGui/imgui_impl_opengl3.h>
#include <ImGui/imgui_internal.h>

#include <GL/glew.h>

#include <shellapi.h>
#include <limits>

static bool cameraOpened = true;
static bool configOpened = true;
static bool consoleOpened = true;
static bool aboutOpened = false;
static bool propertiesOpened = true;

ModuleEditor::ModuleEditor() {}

ModuleEditor::~ModuleEditor() {}

bool ModuleEditor::Init()
{
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;        // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

	windows.push_back(scene = std::make_shared<WindowScene>());
	windows.push_back(std::make_shared<WindowConfiguration>());
	windows.push_back(std::make_shared<WindowConsole>());
	windows.push_back(std::make_shared<WindowHierarchy>());
	windows.push_back(std::make_shared<WindowInspector>());
	
	mainMenu = std::make_unique<WindowMainMenu>(windows);

	return true;
}

bool ModuleEditor::Start()
{
	ImGui_ImplSDL2_InitForOpenGL(App->window->GetWindow(), App->renderer->context);
	ImGui_ImplOpenGL3_Init(GLSL_VERSION);

	return true;
}

bool ModuleEditor::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	lines.clear();
	windows.clear();

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->GetWindow());
	ImGui::NewFrame();
	
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
	update_status status = UPDATE_CONTINUE;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGuiID dock_space_id = ImGui::GetID("DockSpace");
		
	if (!ImGui::DockBuilderGetNode(dock_space_id))
	{
		ImGui::DockBuilderAddNode(dock_space_id);
		ImGui::DockBuilderSetNodeSize(dock_space_id, viewport->WorkSize);

		dock_main_id = dock_space_id;
		dock_left_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.25f, nullptr, &dock_main_id);
		dock_right_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.33f, nullptr, &dock_main_id);
		dock_down_id = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.3f, nullptr, &dock_main_id);
	}

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
	ImGui::DockSpace(dock_space_id);
	ImGui::End();

	mainMenu->Draw();
	for (int i = 0; i < windows.size(); ++i) {
		bool windowEnabled = mainMenu->IsWindowEnabled(i);
		windows[i]->Draw(windowEnabled);
		mainMenu->SetWindowEnabled(i, windowEnabled);
	}

	return status;
}

update_status ModuleEditor::PostUpdate()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backupCurrentWindow = SDL_GL_GetCurrentWindow();
		SDL_GLContext backupCurrentContext = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backupCurrentWindow, backupCurrentContext);
	}

	return UPDATE_CONTINUE;
}

void ModuleEditor::Resized()
{
	windowResized = true;
}

bool ModuleEditor::IsSceneFocused() const
{
	return this->scene->IsFocused();
}
