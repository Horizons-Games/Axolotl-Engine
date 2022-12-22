#include "WindowWindow.h"

#include "imgui.h"

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
	float brightness = App->window->GetBrightness();
	ImGui::TextUnformatted("Window Brightness");
	bool brightnessChanged = ImGui::SliderFloat("##bright", &brightness, .25f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	if (brightnessChanged)
		App->window->SetBrightness(brightness);

	std::pair<int, int> windowDimensions;
	windowDimensions = App->window->GetWindowSize();
	ImGui::TextUnformatted("Window Width");
	bool widthChanged = ImGui::SliderInt("##width", &windowDimensions.first, 640, 1920, "%d", ImGuiSliderFlags_AlwaysClamp);
	ImGui::TextUnformatted("Window Height");
	bool heighChanged = ImGui::SliderInt("##height", &windowDimensions.second, 360, 1080, "%d", ImGuiSliderFlags_AlwaysClamp);
	if (widthChanged || heighChanged)
		App->window->SetWindowSize(windowDimensions.first, windowDimensions.second);

	bool modified = false;

	bool fullscreen = App->window->GetFullscreen();
	if (ImGui::Checkbox("Fullscreen", &fullscreen))
	{
		modified = true;
	}

	ImGui::SameLine();

	bool resizable = App->window->GetResizable();
	if (ImGui::Checkbox("Resizable", &resizable))
	{
		modified = true;
	}
		
	bool borderless = App->window->GetBorderless();
	if (ImGui::Checkbox("Borderless", &borderless))
	{
		modified = true;
	}

	ImGui::SameLine();

	bool fullDesktop = App->window->GetFulscreenDesktop();
	if (ImGui::Checkbox("Desktop Fullscreen", &fullDesktop))
	{
		modified = true;
	}

	if (modified)
	{
		App->window->SetWindowType(
			fullscreen, borderless, resizable, fullDesktop
		);
	}
}
