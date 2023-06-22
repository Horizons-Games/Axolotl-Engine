#include "ModuleEditor.h"

#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"

#include "DataModels/Scene/Scene.h"
#include "FileSystem/ModuleFileSystem.h"

#include "FileSystem/Json.h"

#include "Windows/EditorWindows/WindowStateMachineEditor.h"
#include "Windows/PopUpWindows/WindowLoading.h"
#include "Windows/WindowDebug.h"
#include "Windows/WindowMainMenu.h"
#ifdef ENGINE
	#include "Auxiliar/GameBuilder.h"
	#include "Resources/ResourceStateMachine.h"
	#include "Windows/EditorWindows/WindowAssetFolder.h"
	#include "Windows/EditorWindows/WindowConfiguration.h"
	#include "Windows/EditorWindows/WindowConsole.h"
	#include "Windows/EditorWindows/WindowEditorControl.h"
	#include "Windows/EditorWindows/WindowHierarchy.h"
	#include "Windows/EditorWindows/WindowInspector.h"
	#include "Windows/EditorWindows/WindowResources.h"
	#include "Windows/EditorWindows/WindowScene.h"
	#include "Windows/EditorWindows/WindowNavigation.h"
#else
	#include "Windows/EditorWindows/EditorWindow.h"
#endif

#ifdef DEBUG
	#include "optick.h"
#endif // DEBUG

#include <ImGui/ImGuizmo.h>
#include <ImGui/imgui_impl_opengl3.h>
#include <ImGui/imgui_impl_sdl.h>
#include <ImGui/imgui_internal.h>

#include <FontIcons/CustomFont.cpp>

const std::string ModuleEditor::settingsFolder = "Settings/";
const std::string ModuleEditor::set = "Settings/WindowsStates.conf";

ModuleEditor::ModuleEditor() :
	mainMenu(nullptr),
	scene(nullptr),
	stateMachineWindowEnable(false),
	stateMachineEditor(nullptr)
{
}

ModuleEditor::~ModuleEditor()
{
}

bool ModuleEditor::Init()
{
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;	// Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;	// Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;		// Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; // Prevent mouse flickering

	io.Fonts->AddFontDefault();
	static const ImWchar icons_ranges[] = { ICON_MIN_IGFD, ICON_MAX_IGFD, 0 };
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	io.Fonts->AddFontFromMemoryCompressedBase85TTF(FONT_ICON_BUFFER_NAME_IGFD, 15.0f, &icons_config, icons_ranges);

#ifdef ENGINE
	rapidjson::Document doc;
	Json json(doc, doc);

	windows.push_back(std::unique_ptr<WindowScene>(scene = new WindowScene()));
	windows.push_back(std::make_unique<WindowConfiguration>());
	windows.push_back(std::make_unique<WindowResources>());
	windows.push_back(std::unique_ptr<WindowInspector>(inspector = new WindowInspector()));
	windows.push_back(std::make_unique<WindowHierarchy>());
	windows.push_back(std::make_unique<WindowEditorControl>());
	windows.push_back(std::make_unique<WindowAssetFolder>());
	windows.push_back(std::make_unique<WindowConsole>());
	windows.push_back(std::make_unique<WindowNavigation>());
	
	char* buffer = StateWindows();

	if(buffer == nullptr)
	{		
		rapidjson::StringBuffer newBuffer;
		for (const std::unique_ptr<EditorWindow>& window : windows)
		{
			json[window->GetName().c_str()] = window->DefaultActiveState();
		}
		json.toBuffer(newBuffer);
	}
	else
	{
		json.fromBuffer(buffer);

		auto windowNameNotInJson = [&json](const std::string& windowName)
		{
			std::vector<const char*> namesInJson = json.GetVectorNames();
			return std::none_of(std::begin(namesInJson),
								std::end(namesInJson),
								[&windowName](const char* name)
								{
									return windowName == name;
								});
		};

		for (const std::unique_ptr<EditorWindow>& window : windows)
		{
			if (windowNameNotInJson(window->GetName()))
			{
				json[window->GetName().c_str()] = window->DefaultActiveState();
			}
		}
	}

	delete buffer;
	
	mainMenu = std::make_unique<WindowMainMenu>(json);
	stateMachineEditor = std::make_unique<WindowStateMachineEditor>();
	buildGameLoading = std::make_unique<WindowLoading>();

	ImGuizmo::SetImGuiContext(ImGui::GetCurrentContext());
#else
	debugOptions = std::make_unique<WindowDebug>();
#endif

	return true;
}

bool ModuleEditor::Start()
{
	ImGui_ImplSDL2_InitForOpenGL(App->GetModule<ModuleWindow>()->GetWindow(), App->GetModule<ModuleRender>()->context);
	ImGui_ImplOpenGL3_Init(GLSL_VERSION);
	CreateFolderSettings();
	return true;
}

bool ModuleEditor::CleanUp()
{
#ifdef ENGINE
	rapidjson::Document doc;
	Json json(doc, doc);

	for (int i = 0; i < windows.size(); ++i)
	{
		json[windows[i].get()->GetName().c_str()] = mainMenu.get()->IsWindowEnabled(i);
	}
	rapidjson::StringBuffer buffer;
	json.toBuffer(buffer);
	App->GetModule<ModuleFileSystem>()->Save(set.c_str(), buffer.GetString(), (unsigned int) buffer.GetSize());
	builder::Terminate();
#endif

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->GetModule<ModuleWindow>()->GetWindow());
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
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGuiID dockSpaceId = ImGui::GetID("DockSpace");
	ModuleInput* input = App->GetModule<ModuleInput>();

	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);

	ImGuiWindowFlags dockSpaceWindowFlags = 0;
	dockSpaceWindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
							ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking;
	dockSpaceWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("DockSpace", nullptr, dockSpaceWindowFlags);
	ImGui::PopStyleVar(3);
	ImGui::DockSpace(dockSpaceId);

	static bool firstTime = true;
	if (firstTime && !App->GetModule<ModuleFileSystem>()->Exists("imgui.ini"))
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

	// disable ALT key triggering nav menu
	ImGui::GetCurrentContext()->NavWindowingToggleLayer = false;

	mainMenu->Draw();

	DrawLoadingBuild();

	for (int i = 0; i < windows.size(); ++i)
	{
		bool windowEnabled = mainMenu->IsWindowEnabled(i);
		windows[i]->Draw(windowEnabled);
		mainMenu->SetWindowEnabled(i, windowEnabled);
	}
	stateMachineEditor->Draw(stateMachineWindowEnable);
#else
	debugOptions->Draw();
#endif

	return update_status::UPDATE_CONTINUE;
}

void ModuleEditor::DrawLoadingBuild()
{
#ifdef ENGINE
	bool gameCompiling = builder::Compiling();
	bool zipping = builder::Zipping();
	bool gameBuilding = gameCompiling || zipping;
	if (gameCompiling)
	{
		buildGameLoading->AddWaitingOn("Game is being compiled...");
	}
	if (zipping)
	{
		buildGameLoading->AddWaitingOn("Binaries are being zipped...");
	}
	buildGameLoading->Draw(gameBuilding);
	if (gameBuilding)
	{
		buildGameLoading->ResetWaitingOn();
	}
#endif
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

void ModuleEditor::SetStateMachineWindowEditor(const std::weak_ptr<ResourceStateMachine>& resource)
{
#ifdef ENGINE
	this->stateMachineEditor->SetStateMachine(resource);
	stateMachineWindowEnable = true;
#endif
}

void ModuleEditor::SetResourceOnStateMachineEditor(const std::shared_ptr<Resource>& resource)
{
#ifdef ENGINE
	stateMachineEditor->SetResourceOnState(resource);
#endif
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

void ModuleEditor::RefreshInspector() const
{
#ifdef ENGINE
	inspector->ResetSelectedGameObject();
#endif // ENGINE
}

std::pair<float, float> ModuleEditor::GetAvailableRegion()
{
#ifdef ENGINE
	ImVec2 region = scene->GetAvailableRegion();
	return std::make_pair(region.x, region.y);
#else
	return App->GetModule<ModuleWindow>()->GetWindowSize();
#endif
}

char* ModuleEditor::StateWindows()
{
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
	if (fileSystem->Exists(settingsFolder.c_str()))
	{
		if (fileSystem->Exists(set.c_str()))
		{
			char* binaryBuffer = {};
			fileSystem->Load(set.c_str(), binaryBuffer);
			return binaryBuffer;
		}
	}
	return nullptr;
}

void ModuleEditor::CreateFolderSettings()
{
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();
	bool settingsFolderNotCreated = !fileSystem->Exists(settingsFolder.c_str());
	if (settingsFolderNotCreated)
	{
		fileSystem->CreateDirectory(settingsFolder.c_str());
	}
}
