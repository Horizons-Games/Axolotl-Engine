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
	float brightness = App->GetModule<ModuleWindow>()->GetBrightness();
	ImGui::TextUnformatted("Window Brightness");
	bool brightnessChanged =
		ImGui::SliderFloat("##bright", &brightness, .25f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	if (brightnessChanged)
	{
		App->GetModule<ModuleWindow>()->SetBrightness(brightness);
	}

	std::pair<int, int> windowDimensions;
	windowDimensions = App->GetModule<ModuleWindow>()->GetWindowSize();
	ImGui::TextUnformatted("Window Width");
	bool widthChanged =
		ImGui::SliderInt("##width", &windowDimensions.first, 640, 1920, "%d", ImGuiSliderFlags_AlwaysClamp);
	ImGui::TextUnformatted("Window Height");
	bool heighChanged =
		ImGui::SliderInt("##height", &windowDimensions.second, 360, 1080, "%d", ImGuiSliderFlags_AlwaysClamp);
	if (widthChanged || heighChanged)
	{
		App->GetModule<ModuleWindow>()->SetWindowSize(windowDimensions.first, windowDimensions.second);
	}

	bool fullscreen = App->GetModule<ModuleWindow>()->IsWindowFullscreen();
	if (ImGui::Checkbox("Fullscreen", &fullscreen))
	{
		App->GetModule<ModuleWindow>()->SetFullscreen(fullscreen);
	}

	ImGui::SameLine();

	bool resizable = App->GetModule<ModuleWindow>()->IsWindowResizable();
	if (ImGui::Checkbox("Resizable", &resizable))
	{
		App->GetModule<ModuleWindow>()->SetResizable(resizable);
	}

	bool borderless = App->GetModule<ModuleWindow>()->IsWindowBorderless();
	if (ImGui::Checkbox("Borderless", &borderless))
	{
		App->GetModule<ModuleWindow>()->SetBorderless(borderless);
	}

	ImGui::SameLine();

	bool fullDesktop = App->GetModule<ModuleWindow>()->IsWindowDesktopFullscreen();
	if (ImGui::Checkbox("Desktop Fullscreen", &fullDesktop))
	{
		App->GetModule<ModuleWindow>()->SetDesktopFullscreen(fullDesktop);
	}
}
