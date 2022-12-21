#include "Application.h"
#include "Globals.h"

#include "ModuleEditor.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleEngineCamera.h"
#include "ModuleTexture.h"

#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_sdl.h>
#include <ImGui/imgui_impl_opengl3.h>

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
	//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

	return true;
}

bool ModuleEditor::Start()
{
	ImGui_ImplSDL2_InitForOpenGL(App->window->window, App->renderer->context);
	ImGui_ImplOpenGL3_Init(GLSL_VERSION);

	return true;
}

bool ModuleEditor::CleanUp()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	lines.clear();

	return true;
}

update_status ModuleEditor::PreUpdate()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->window->window);
	ImGui::NewFrame();
	
	return UPDATE_CONTINUE;
}

update_status ModuleEditor::Update()
{
	update_status status = UPDATE_CONTINUE;

	std::pair<int, int>&& window = App->window->GetWindowSize();

	//ImGui::ShowDemoWindow();

	if (ImGui::BeginMainMenuBar())
	{		 
		if (ImGui::BeginMenu("Window"))
		{
			ImGui::MenuItem("Configuration", NULL, &configOpened);
			ImGui::MenuItem("Camera", NULL, &cameraOpened);

			ImGui::EndMenu();
		}

		ImGui::MenuItem("Properties", NULL, &propertiesOpened);

		ImGui::MenuItem("Console", NULL, &consoleOpened);

		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Github page"))
				ShellExecute(0, 0, "https://github.com/MacXxs/Engine", 0, 0, SW_SHOW);

			ImGui::MenuItem("About", NULL, &aboutOpened);

			ImGui::EndMenu();
		}
		
		if (ImGui::MenuItem("Quit")) status = UPDATE_STOP;

		ImGui::EndMainMenuBar();
	}

	if (cameraOpened)
	{
		ImGui::SetNextWindowPos(ImVec2(10, 30), ImGuiCond_Once);

		if (ImGui::Begin("Camera Settings", &cameraOpened, ImGuiWindowFlags_AlwaysAutoResize))
		{
			int hfov = App->engineCamera->GetHFOV();
			int vfov = App->engineCamera->GetVFOV();
			float znear = App->engineCamera->GetZNear();
			float zfar = App->engineCamera->GetZFar();

			if (ImGui::CollapsingHeader("Frustum", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::SliderInt("Horizontal FOV", &hfov, MIN_HFOV, MAX_HFOV))
					App->engineCamera->SetHFOV(math::DegToRad(hfov));
				if (ImGui::SliderInt("Vertical FOV", &vfov, MIN_VFOV, MAX_VFOV))
					App->engineCamera->SetVFOV(math::DegToRad(vfov));

				if (ImGui::InputFloat("Z near", &znear, 0.5f, 0.0f))
					App->engineCamera->SetPlaneDistance(znear, zfar);
				if (ImGui::InputFloat("Z far", &zfar, 0.5f, 0.0f))
					App->engineCamera->SetPlaneDistance(znear, zfar);
			}

			float movementSpeed = App->engineCamera->GetMoveSpeed();
			float rotationSpeed = App->engineCamera->GetRotationSpeed();

			if (ImGui::CollapsingHeader("Movement", ImGuiTreeNodeFlags_DefaultOpen))
			{
				if (ImGui::SliderFloat("Movement Speed", &movementSpeed,
					DEFAULT_MOVE_SPEED, DEFAULT_MOVE_SPEED * 10.f))
					App->engineCamera->SetMoveSpeed(movementSpeed);
				if (ImGui::SliderFloat("Rotation Speed (keyboard)", &rotationSpeed,
					DEFAULT_ROTATION_SPEED, DEFAULT_ROTATION_SPEED * 10.f))
					App->engineCamera->SetRotationSpeed(rotationSpeed);
			}

			static float4 color = App->renderer->GetBackgroundColor();

			if (ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Text("Background Color");
				if (ImGui::ColorEdit3("MyColor##1", (float*)&color))
					App->renderer->SetBackgroundColor(color);
			}
		}

		ImGui::End();
	}

	if (configOpened)
	{
		ImGui::SetNextWindowPos(ImVec2(window.first - 340, 30), ImGuiCond_Once);

		if (windowResized) ImGui::SetNextWindowPos(ImVec2(window.first - 340, 30));

		if (ImGui::Begin("Configuration", &configOpened, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (ImGui::CollapsingHeader("Application", ImGuiTreeNodeFlags_DefaultOpen))
			{
				char title[25];
				int maxFPS = App->GetMaxFrameRate();

				ImGui::Text("Engine name: "); 
				ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), TITLE);
				
				if (ImGui::SliderInt("Max FPS", &maxFPS, 0, 100))
					App->SetMaxFrameRate(maxFPS);

				sprintf_s(title, 25, "Framerate %.1f", App->fpsLog[App->fpsLog.size() - 1]);
				ImGui::PlotHistogram("##framerate", &App->fpsLog[0], App->fpsLog.size(), 0, 
					title, 0.0f, 100.0f, ImVec2(310, 100));
				sprintf_s(title, 25, "Milliseconds %0.1f", App->msLog[App->msLog.size() - 1]);
				ImGui::PlotHistogram("##milliseconds", &App->msLog[0], App->msLog.size(), 0,
					title, 0.0f, 40.0f, ImVec2(310, 100));
			}

			if (ImGui::CollapsingHeader("Window", ImGuiTreeNodeFlags_DefaultOpen))
			{
				std::pair<int, int> windowDimensions;
				windowDimensions = App->window->GetWindowSize();

				bool fullscreen = App->window->GetFullscreen();
				bool borderless = App->window->GetBorderless();
				bool resizable = App->window->GetResizable();
				bool fullscreenDsktp = App->window->GetFulscreenDesktop();
				bool modified = false;

				float brightness = App->window->GetBrightness();

				SDL_DisplayMode DM;
				SDL_GetCurrentDisplayMode(0, &DM);

				if (ImGui::SliderInt("Width", &windowDimensions.first, 320, DM.w))
					App->window->SetWindowSize(windowDimensions.first, windowDimensions.second);
				if (ImGui::SliderInt("Height", &windowDimensions.second, 160, DM.h))
					App->window->SetWindowSize(windowDimensions.first, windowDimensions.second);

				if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
					App->window->SetBrightness(brightness);

				if (ImGui::BeginTable("WindowTable1", 2))
				{
					ImGui::TableNextColumn(); 
					if (ImGui::Checkbox("Fullscreen", &fullscreen)) modified = true;
					ImGui::TableNextColumn();
					if (ImGui::Checkbox("Borderless", &borderless)) modified = true;
					ImGui::TableNextColumn();
					if (ImGui::Checkbox("Resizable", &resizable)) modified = true;
					ImGui::TableNextColumn();
					if (ImGui::Checkbox("Full Desktop", &fullscreenDsktp)) modified = true;

					ImGui::EndTable();
				}
				
				if (modified)
				{
					App->window->SetWindowType(
						fullscreen, borderless, resizable, fullscreenDsktp
					);
				}
			}

			if (ImGui::CollapsingHeader("Hardware", ImGuiTreeNodeFlags_DefaultOpen))
			{
				SDL_version version = App->renderer->GetSDLVersion();

				ImGui::Text("SDL Version: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%u.%u.%u", 
					version.major, version.minor, version.patch);
				ImGui::Text("OpenGL version: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glGetString(GL_VERSION));
				ImGui::Text("GLSL: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", 
					glGetString(GL_SHADING_LANGUAGE_VERSION));

				ImGui::Separator();

				ImGui::Text("CPUs: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i (Cache: %iKB)",
					App->renderer->GetCPUCount(), App->renderer->GetCacheLineSize());

				ImGui::Text("System RAM: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.2f GB", App->renderer->GetRamGB());

				ImGui::Text("Caps: "); ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
				ImGui::TextWrapped(App->renderer->GetCaps());
				ImGui::PopStyleColor(1);

				ImGui::Separator();

				ImGui::Text("GPU: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%s", glGetString(GL_VENDOR));
				ImGui::Text("Brand: "); ImGui::SameLine();
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
				ImGui::TextWrapped("%s", glGetString(GL_RENDERER));
				ImGui::PopStyleColor(1);
				
				GLint ramBudget, ramUsage, ramAvailable, ramReserved;
				glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &ramBudget);
				glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &ramAvailable);
				glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &ramReserved);
				ramUsage = ramBudget - ramAvailable;

				ImGui::Text("VRAM Budget: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.2f MB", ramBudget / 1024.0f);
				ImGui::Text("VRAM Usage: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.2f MB", ramUsage / 1024.0f);
				ImGui::Text("VRAM Available: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.2f MB", ramAvailable / 1024.0f);
				ImGui::Text("VRAM Reserved: "); ImGui::SameLine();
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%0.2f MB", ramReserved / 1024.0f);
			}
		}

		ImGui::End();
	}

	if (consoleOpened)
	{
		ImGui::SetNextWindowPos(ImVec2(10, window.second - 280), ImGuiCond_Once);

		if (windowResized) ImGui::SetNextWindowPos(ImVec2(10, window.second - 280));

		if (ImGui::Begin("Console log", &consoleOpened))
		{
			ImGui::SetWindowSize("Console log", ImVec2(900, 250), ImGuiCond_Once);

			std::string logInfo = "";

			for (int i = 0; i < engineLog->logLines.size(); ++i)
			{
				logInfo = engineLog->logLines[i].c_str();
				lines.push_back(logInfo);
			}

			engineLog->logLines.clear();

			for (std::string line : lines)
			{
				ImGui::TextUnformatted(&line[0]);
			}

			if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
				ImGui::SetScrollHereY(1.0f);
		}

		ImGui::End();
	}

	if (aboutOpened)
	{
		ImGui::SetNextWindowPos(ImVec2(window.first / 2 - 150, 30), ImGuiCond_Once);

		if (windowResized) ImGui::SetNextWindowPos(ImVec2(window.first / 2 - 150, 30));

		if (ImGui::Begin("About", &aboutOpened, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
		{
			ImGui::SetWindowSize("About", ImVec2(300, 200));

			ImGui::Text("%s - Version %s", TITLE, VERSION);
			ImGui::Dummy(ImVec2(0.0f, 2.5f));
			ImGui::Text("Author: Miquel Prieto Moline");
			ImGui::Dummy(ImVec2(0.0f, 2.5f));
			ImGui::TextWrapped("Engine developed during the Masters degree in AAA videogame \
development at UPC School.");

			ImGui::End();
		}
	}

	if (propertiesOpened)
	{
		ImGui::SetNextWindowPos(ImVec2(10, window.second / 3), ImGuiCond_Once);

		if (windowResized) ImGui::SetNextWindowPos(ImVec2(10, window.second / 3));

		if (ImGui::Begin("Properties", &propertiesOpened, ImGuiWindowFlags_AlwaysAutoResize))
		{
			if (App->renderer->AnyModelLoaded())
			{
				Model* model = App->renderer->GetModel(0);
				float3 translation = model->GetTranslation();
				float3 scale = model->GetScale();
				float3 rotation = model->GetRotationF3();

				ImGui::Text("TRANSFORMATION");
				ImGui::Dummy(ImVec2(0.0f, 2.5f));

				if (ImGui::BeginTable("TransformTable", 2))
				{
					ImGui::TableNextColumn();
					ImGui::Text("Translation"); ImGui::SameLine();

					ImGui::TableNextColumn();
					ImGui::Text("x:"); ImGui::SameLine();
					ImGui::SetNextItemWidth(80.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
					ImGui::DragFloat("##XTrans", &translation.x, 0.01f,
						std::numeric_limits<float>::min(), std::numeric_limits<float>::min()
					); ImGui::PopStyleVar(); ImGui::SameLine();

					ImGui::Text("y:"); ImGui::SameLine();
					ImGui::SetNextItemWidth(80.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
					ImGui::DragFloat("##YTrans", &translation.y, 0.01f,
						std::numeric_limits<float>::min(), std::numeric_limits<float>::min()
					); ImGui::PopStyleVar(); ImGui::SameLine();

					ImGui::Text("z:"); ImGui::SameLine();
					ImGui::SetNextItemWidth(80.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
					ImGui::DragFloat("##ZTrans", &translation.z, 0.01f,
						std::numeric_limits<float>::min(), std::numeric_limits<float>::min()
					); ImGui::PopStyleVar();

					ImGui::TableNextColumn();
					ImGui::Text("Rotation"); ImGui::SameLine();

					ImGui::TableNextColumn();
					ImGui::Text("x:"); ImGui::SameLine();
					ImGui::SetNextItemWidth(80.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
					ImGui::DragFloat("##XRot", &rotation.x, 0.01f,
						std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%0.3f");
					ImGui::PopStyleVar(); ImGui::SameLine();

					ImGui::Text("y:"); ImGui::SameLine();
					ImGui::SetNextItemWidth(80.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
					ImGui::DragFloat("##YRot", &rotation.y, 0.01f,
						std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%0.3f");
					ImGui::PopStyleVar(); ImGui::SameLine();

					ImGui::Text("z:"); ImGui::SameLine();
					ImGui::SetNextItemWidth(80.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
					ImGui::DragFloat("##ZRot", &rotation.z, 0.01f,
						std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), "%0.3f");
					ImGui::PopStyleVar();

					ImGui::TableNextColumn();
					ImGui::Text("Scale"); ImGui::SameLine();

					ImGui::TableNextColumn();
					ImGui::Text("x:"); ImGui::SameLine();
					ImGui::SetNextItemWidth(80.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
					ImGui::DragFloat("##XScale", &scale.x, 0.01f,
						std::numeric_limits<float>::min(), std::numeric_limits<float>::min()
					); ImGui::PopStyleVar(); ImGui::SameLine();

					ImGui::Text("y:"); ImGui::SameLine();
					ImGui::SetNextItemWidth(80.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
					ImGui::DragFloat("##YScale", &scale.y, 0.01f,
						std::numeric_limits<float>::min(), std::numeric_limits<float>::min()
					); ImGui::PopStyleVar(); ImGui::SameLine();

					ImGui::Text("z:"); ImGui::SameLine();
					ImGui::SetNextItemWidth(80.0f);
					ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(5.0f, 1.0f));
					ImGui::DragFloat("##ZScale", &scale.z, 0.01f,
						std::numeric_limits<float>::min(), std::numeric_limits<float>::min()
					); ImGui::PopStyleVar();

					model->SetTranslation(translation);
					model->SetRotation(rotation);
					model->SetScale(scale);

					ImGui::EndTable();
				}

				
				ImGui::Separator();
				
				ImGui::Text("GEOMETRY");
				ImGui::Dummy(ImVec2(0.0f, 2.5f));
				if (ImGui::BeginTable("GeometryTable1", 2))
				{
					ImGui::TableNextColumn();
					ImGui::Text("Number of vertices: ");
					ImGui::TableNextColumn();
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", model->GetNumVertices());
					ImGui::TableNextColumn();
					ImGui::Text("Number of triangles: ");
					ImGui::TableNextColumn();
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", model->GetNumTriangles());
				
					ImGui::EndTable();
				}
				ImGui::Separator();
				
				ImGui::Text("TEXTURE");
				ImGui::Dummy(ImVec2(0.0f, 2.5f));
				if (ImGui::BeginTable("TextureTable1", 2))
				{
					ImGui::TableNextColumn();
					ImGui::Text("Height: ");
					ImGui::TableNextColumn();
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", App->textures->GetHeight());
					ImGui::TableNextColumn();
					ImGui::Text("Width: ");
					ImGui::TableNextColumn();
					ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%i", App->textures->GetWidth());
				
					ImGui::EndTable();
				}
				ImGui::Image((void*)model->GetTextureId(0), ImVec2(100.0f, 100.0f));
			}

			else ImGui::Text("There's no model loaded in scene.");
		}
		ImGui::End();
	}

	if (windowResized) windowResized = false;

	return status;
}

update_status ModuleEditor::PostUpdate()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	SDL_Window* backupCurrentWindow = SDL_GL_GetCurrentWindow();
	SDL_GLContext backupCurrentContext = SDL_GL_GetCurrentContext();
	ImGui::UpdatePlatformWindows();
	ImGui::RenderPlatformWindowsDefault();
	SDL_GL_MakeCurrent(backupCurrentWindow, backupCurrentContext);

	return UPDATE_CONTINUE;
}

void ModuleEditor::Resized()
{
	windowResized = true;
}
