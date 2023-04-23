#include "ModuleEditor.h"

#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleScene.h"
#include "ModuleInput.h"

#include "DataModels/Scene/Scene.h"
#include "DataStructures/Quadtree.h"
#include "FileSystem/ModuleFileSystem.h"

#include "FileSystem/Json.h"

#include "Windows/WindowMainMenu.h"
#include "Windows/WindowDebug.h"
#ifdef ENGINE
#include "Windows/EditorWindows/WindowConsole.h"
#include "Windows/EditorWindows/WindowScene.h"
#include "Windows/EditorWindows/WindowConfiguration.h"
#include "Windows/EditorWindows/WindowInspector.h"
#include "Windows/EditorWindows/WindowHierarchy.h"
#include "Windows/EditorWindows/WindowEditorControl.h"
#include "Windows/EditorWindows/WindowResources.h"
#include "Windows/EditorWindows/WindowAssetFolder.h"
#else
#include "Windows/EditorWindows/EditorWindow.h"
#endif

#ifdef DEBUG
#include "optick.h"
#endif // DEBUG

#include <ImGui/imgui_internal.h>
#include <ImGui/imgui_impl_sdl.h>
#include <ImGui/imgui_impl_opengl3.h>
#include <ImGui/ImGuizmo.h>

#include <FontIcons/CustomFont.cpp>

const std::string ModuleEditor::settingsFolder = "Settings/";
const std::string ModuleEditor::set = "Settings/WindowsStates.conf";

ModuleEditor::ModuleEditor() : mainMenu(nullptr), scene(nullptr), windowResized(false), copyObject(nullptr)
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
	
	std::string buffer = StateWindows();
	if(buffer.empty())
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
		char* bufferPointer = buffer.data();
		json.fromBuffer(bufferPointer);

		auto windowNameNotInJson = [&json](const std::string& windowName)
		{
			std::vector<const char*> namesInJson = json.GetVectorNames();
			return std::none_of(std::begin(namesInJson), std::end(namesInJson), [&windowName](const char* name)
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
	
	mainMenu = std::make_unique<WindowMainMenu>(json);
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
	App->fileSystem->Save(set.c_str(), buffer.GetString(), (unsigned int)buffer.GetSize());
#endif

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

	if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::REPEAT 
		|| App->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::DOWN)
		&& App->input->GetKey(SDL_SCANCODE_C) == KeyState::DOWN)
	{
		CopyAnObject();
	}
	
	if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::REPEAT
		|| App->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::DOWN)
		&& App->input->GetKey(SDL_SCANCODE_V) == KeyState::DOWN)
	{
		PasteAnObject();
	}

	if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::REPEAT
		|| App->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::DOWN)
		&& App->input->GetKey(SDL_SCANCODE_X) == KeyState::DOWN)
	{
		CutAnObject();
	}

	if ((App->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::REPEAT
		|| App->input->GetKey(SDL_SCANCODE_LCTRL) == KeyState::DOWN)
		&& App->input->GetKey(SDL_SCANCODE_D) == KeyState::DOWN)
	{
		DuplicateAnObject();
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

void ModuleEditor::CopyAnObject()
{
	if (App->scene->GetSelectedGameObject() != App->scene->GetLoadedScene()->GetRoot() 
		&& App->scene->GetSelectedGameObject() != App->scene->GetLoadedScene()->GetAmbientLight() 
		&& App->scene->GetSelectedGameObject() != App->scene->GetLoadedScene()->GetDirectionalLight())
	{
		copyObject = std::make_unique<GameObject>(*App->scene->GetSelectedGameObject());
	}
	
}

void ModuleEditor::PasteAnObject()
{
	if(copyObject)
	{
		if (App->scene->GetSelectedGameObject())
		{
			App->scene->GetLoadedScene()->
				DuplicateGameObject(copyObject->GetName(), copyObject.get(), App->scene->GetSelectedGameObject());
		}
		else
		{
			App->scene->GetLoadedScene()->
				DuplicateGameObject(copyObject->GetName(), copyObject.get(), App->scene->GetLoadedScene()->GetRoot());
		}
	}
}

void ModuleEditor::CutAnObject()
{
	CopyAnObject();

	GameObject* gameObject = App->scene->GetSelectedGameObject();
	App->scene->SetSelectedGameObject(gameObject->GetParent()); // If a GameObject is destroyed, 
																			// change the focus to its parent
	App->scene->GetLoadedScene()->GetRootQuadtree()->
		RemoveGameObjectAndChildren(gameObject->GetParent());

	App->scene->GetLoadedScene()->DestroyGameObject(gameObject);
}

void ModuleEditor::DuplicateAnObject()
{
	if (App->scene->GetSelectedGameObject() 
		&& App->scene->GetSelectedGameObject() != App->scene->GetLoadedScene()->GetRoot()
		&& App->scene->GetSelectedGameObject() != App->scene->GetLoadedScene()->GetAmbientLight()
		&& App->scene->GetSelectedGameObject() != App->scene->GetLoadedScene()->GetDirectionalLight())
	{
		App->scene->GetLoadedScene()->
			DuplicateGameObject(App->scene->GetSelectedGameObject()->GetName()
				, App->scene->GetSelectedGameObject(), App->scene->GetSelectedGameObject()->GetParent());
	}
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
	return App->window->GetWindowSize();
#endif
}
std::string ModuleEditor::StateWindows()
{
	if (App->fileSystem->Exists(settingsFolder.c_str()))
	{		
		if (App->fileSystem->Exists(set.c_str()))
		{
			char* binaryBuffer = {};
			App->fileSystem->Load(set.c_str(), binaryBuffer);
			return std::string(binaryBuffer);
		}
	}
	return std::string();
}

void ModuleEditor::CreateFolderSettings()
{
	bool settingsFolderNotCreated = !App->fileSystem->Exists(settingsFolder.c_str());
	if (settingsFolderNotCreated)
	{
		App->fileSystem->CreateDirectoryA(settingsFolder.c_str());
	}
}
