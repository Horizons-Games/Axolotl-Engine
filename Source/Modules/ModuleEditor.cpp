#include "ModuleEditor.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "FileSystem/ModuleFileSystem.h"

#ifdef ENGINE
#include "Windows/WindowMainMenu.h"
#include "Windows/EditorWindows/WindowConsole.h"
#include "Windows/EditorWindows/WindowScene.h"
#include "Windows/EditorWindows/WindowConfiguration.h"
#include "Windows/EditorWindows/WindowInspector.h"
#include "Windows/EditorWindows/WindowHierarchy.h"
#include "Windows/EditorWindows/WindowEditorControl.h"
#include "Windows/EditorWindows/WindowResources.h"
#else
#include "Windows/WindowDebug.h"
#endif

#ifdef DEBUG
#include "optick.h"
#endif // DEBUG

#include <ImGui/imgui_internal.h>
#include <ImGui/imgui_impl_sdl.h>
#include <ImGui/imgui_impl_opengl3.h>
#include <ImGui/ImGuizmo.h>

#include <FontIcons/CustomFont.cpp>

static bool cameraOpened = true;
static bool configOpened = true;
static bool consoleOpened = true;
static bool aboutOpened = false;
static bool propertiesOpened = true;

ModuleEditor::ModuleEditor() : scene(nullptr), windowResized(false)
{
}

ModuleEditor::~ModuleEditor() 
{
}

bool ModuleEditor::Init()
{
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;        // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;     // Prevent mouse flickering

	io.Fonts->AddFontDefault();
	static const ImWchar icons_ranges[] = { ICON_MIN_IGFD, ICON_MAX_IGFD, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
	io.Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_IGFD, 15.0f, &icons_config, icons_ranges);

#ifdef ENGINE
	windows.push_back(std::unique_ptr<WindowScene>(scene = new WindowScene()));
	windows.push_back(std::make_unique<WindowConfiguration>());
	windows.push_back(std::make_unique<WindowResources>());
	windows.push_back(std::unique_ptr<WindowInspector>(inspector = new WindowInspector()));
	windows.push_back(std::make_unique<WindowHierarchy>());
	windows.push_back(std::make_unique<WindowEditorControl>());
	windows.push_back(std::make_unique<WindowFileBrowser>());
	windows.push_back(std::make_unique<WindowConsole>());
	mainMenu = std::make_unique<WindowMainMenu>(windows);

	ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
#else
	debugOptions = std::make_unique<WindowDebug>();
#endif

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

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->GetWindow());
	ImGui::NewFrame();

#ifdef ENGINE
	ImGuizmo::BeginFrame();
	ImGuizmo::Enable(true);
#endif
	
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
#ifdef DEBUG
	OPTICK_CATEGORY("UpdateEditor", Optick::Category::UI);
#endif // DEBUG

#ifdef ENGINE
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

	static bool firstTime = true;
	if (firstTime && !App->fileSystem->Exists("imgui.ini"))
	{
		firstTime = false;

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

	ImGui::End();

	//disable ALT key triggering nav menu
	ImGui::GetCurrentContext()->NavWindowingToggleLayer = false;

	mainMenu->Draw();
	for (int i = 0; i < windows.size(); ++i) {
		bool windowEnabled = mainMenu->IsWindowEnabled(i);
		windows[i]->Draw(windowEnabled);
		mainMenu->SetWindowEnabled(i, windowEnabled);
	}
#else
	debugOptions->Draw();
#endif

	return update_status::UPDATE_CONTINUE;
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

	return update_status::UPDATE_CONTINUE;
}

void ModuleEditor::Resized()
{
	windowResized = true;
}

bool ModuleEditor::IsSceneFocused() const
{
#ifdef ENGINE
	return scene->IsFocused();
#else
	return true;
#endif
}

void ModuleEditor::SetResourceOnInspector(const std::weak_ptr<Resource>& resource) const
{
#ifdef ENGINE
	this->inspector->SetResource(resource);
#endif
}
