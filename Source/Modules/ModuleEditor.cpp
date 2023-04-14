#include "ModuleEditor.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"

#include "FileSystem/Json.h"
#include "FileSystem/ModuleFileSystem.h"

#include "Windows/WindowMainMenu.h"
#include "Windows/EditorWindows/WindowConsole.h"
#include "Windows/EditorWindows/WindowScene.h"
#include "Windows/EditorWindows/WindowConfiguration.h"
#include "Windows/EditorWindows/WindowInspector.h"
#include "Windows/EditorWindows/WindowHierarchy.h"
#include "Windows/EditorWindows/WindowEditorControl.h"

#include "optick.h"

#include <ImGui/imgui_internal.h>
#include <ImGui/imgui_impl_sdl.h>
#include <ImGui/imgui_impl_opengl3.h>

#include <FontIcons/CustomFont.cpp>

static bool cameraOpened = true;
static bool configOpened = true;
static bool consoleOpened = true;
static bool aboutOpened = false;
static bool propertiesOpened = true;
const std::string ModuleEditor::settingsFolder = "Settings/";
const std::string ModuleEditor::set = "Settings/WindowsStates.conf";

ModuleEditor::ModuleEditor() : mainMenu(nullptr), scene(nullptr), windowResized(false)
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

	rapidjson::Document doc;
	Json json(doc, doc);
	char* buffer = StateWindows();
	
	windows.push_back(std::unique_ptr<WindowScene>(scene = new WindowScene()));
	windows.push_back(std::make_unique<WindowConfiguration>());
	windows.push_back(std::make_unique<WindowInspector>());
	windows.push_back(std::make_unique<WindowHierarchy>());
	windows.push_back(std::make_unique<WindowEditorControl>());
	windows.push_back(std::make_unique<WindowFileBrowser>());
	windows.push_back(std::make_unique<WindowConsole>());	
	if(buffer==nullptr)
	{		
		rapidjson::StringBuffer newBuffer;
		for (int i = 0; i < windows.size(); ++i)
		{
			json[windows[i].get()->GetName().c_str()]=true;
		}
		json.toBuffer(newBuffer);
	}
	else
	{
		json.fromBuffer(buffer);
	}
	
	mainMenu = std::make_unique<WindowMainMenu>(json);
	delete buffer;
	return true;
}

bool ModuleEditor::Start()
{
	ImGui_ImplSDL2_InitForOpenGL(App->window->GetWindow(), App->renderer->context);
	ImGui_ImplOpenGL3_Init(GLSL_VERSION);
	CreateFolderSettings();
	return true;
}

bool ModuleEditor::CleanUp()
{
	rapidjson::Document doc;
	Json json(doc, doc);	
	
	for (int i = 0; i < windows.size(); ++i) 
	{
		json[windows[i].get()->GetName().c_str()] = mainMenu.get()->IsWindowEnabled(i);				
	}
	rapidjson::StringBuffer buffer;
	json.toBuffer(buffer);	
	App->fileSystem->Save(set.c_str(), buffer.GetString(), (unsigned int)buffer.GetSize());
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
	
	windows.clear();

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->GetWindow());
	ImGui::NewFrame();
	
	return update_status::UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
	OPTICK_CATEGORY("UpdateEditor", Optick::Category::UI);

	update_status status = update_status::UPDATE_CONTINUE;

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
	ImGui::End();

	//disable ALT key triggering nav menu
	ImGui::GetCurrentContext()->NavWindowingToggleLayer = false;

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

	return update_status::UPDATE_CONTINUE;
}

void ModuleEditor::Resized()
{
	windowResized = true;
}

bool ModuleEditor::IsSceneFocused() const
{
	return scene->IsFocused();
}

char* ModuleEditor::StateWindows()
{
	char* binaryBuffer = {};
	if (App->fileSystem->Exists(settingsFolder.c_str()))
	{		
		if (App->fileSystem->Exists(set.c_str()))
		{
			App->fileSystem->Load(set.c_str(), binaryBuffer);
		}
	}
	return binaryBuffer;

}

void ModuleEditor::CreateFolderSettings()
{
	bool settingsFolderNotCreated = !App->fileSystem->Exists(settingsFolder.c_str());
	if (settingsFolderNotCreated)
	{
		App->fileSystem->CreateDirectoryA(settingsFolder.c_str());
	}
}
