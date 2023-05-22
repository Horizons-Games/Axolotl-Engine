#include "WindowWindow.h"

#include "Application.h"
#include "Modules/ModuleWindow.h"

WindowWindow::WindowWindow() : SubWindow("Window")
{
}

WindowWindow::~WindowWindow()
{
}

void WindowWindow::DrawWindowContents()
{
	ModuleWindow* window = App->GetModule<ModuleWindow>();
	float brightness = window->GetBrightness();
	ImGui::TextUnformatted("Window Brightness");
	bool brightnessChanged =
		ImGui::SliderFloat("##bright", &brightness, .25f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	if (brightnessChanged)
	{
		window->SetBrightness(brightness);
	}

	std::pair<int, int> windowDimensions;
	windowDimensions = window->GetWindowSize();
	ImGui::TextUnformatted("Window Width");
	bool widthChanged =
		ImGui::SliderInt("##width", &windowDimensions.first, 640, 1920, "%d", ImGuiSliderFlags_AlwaysClamp);
	ImGui::TextUnformatted("Window Height");
	bool heighChanged =
		ImGui::SliderInt("##height", &windowDimensions.second, 360, 1080, "%d", ImGuiSliderFlags_AlwaysClamp);
	if (widthChanged || heighChanged)
	{
		window->SetWindowSize(windowDimensions.first, windowDimensions.second);
	}

	bool fullscreen = window->IsWindowFullscreen();
	if (ImGui::Checkbox("Fullscreen", &fullscreen))
	{
		window->SetFullscreen(fullscreen);
	}

	ImGui::SameLine();

	bool resizable = window->IsWindowResizable();
	if (ImGui::Checkbox("Resizable", &resizable))
	{
		window->SetResizable(resizable);
	}

	bool borderless = window->IsWindowBorderless();
	if (ImGui::Checkbox("Borderless", &borderless))
	{
		window->SetBorderless(borderless);
	}

	ImGui::SameLine();

	bool fullDesktop = window->IsWindowDesktopFullscreen();
	if (ImGui::Checkbox("Desktop Fullscreen", &fullDesktop))
	{
		window->SetDesktopFullscreen(fullDesktop);
	}
}
