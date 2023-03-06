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
	float brightness = App->GetModuleWindow()->GetBrightness();
	ImGui::TextUnformatted("Window Brightness");
	bool brightnessChanged = 
		ImGui::SliderFloat("##bright", &brightness, .25f, 1.f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
	if (brightnessChanged)
	{
		App->window->SetBrightness(brightness);
	}

	std::pair<int, int> windowDimensions;
	windowDimensions = App->GetModuleWindow()->GetWindowSize();
	ImGui::TextUnformatted("Window Width");
	bool widthChanged = 
		ImGui::SliderInt("##width", &windowDimensions.first, 640, 1920, "%d", ImGuiSliderFlags_AlwaysClamp);
	ImGui::TextUnformatted("Window Height");
	bool heighChanged = 
		ImGui::SliderInt("##height", &windowDimensions.second, 360, 1080, "%d", ImGuiSliderFlags_AlwaysClamp);
	if (widthChanged || heighChanged)
	{
		App->window->SetWindowSize(windowDimensions.first, windowDimensions.second);
	}

	bool fullscreen = App->GetModuleWindow()->IsWindowFullscreen();
	if (ImGui::Checkbox("Fullscreen", &fullscreen))
	{
		App->GetModuleWindow()->SetFullscreen(fullscreen);
	}

	ImGui::SameLine();

	bool resizable = App->GetModuleWindow()->IsWindowResizable();
	if (ImGui::Checkbox("Resizable", &resizable))
	{
		App->GetModuleWindow()->SetResizable(resizable);
	}
		
	bool borderless = App->GetModuleWindow()->IsWindowBorderless();
	if (ImGui::Checkbox("Borderless", &borderless))
	{
		App->GetModuleWindow()->SetBorderless(borderless);
	}

	ImGui::SameLine();

	bool fullDesktop = App->GetModuleWindow()->IsWindowDesktopFullscreen();
	if (ImGui::Checkbox("Desktop Fullscreen", &fullDesktop))
	{
		App->GetModuleWindow()->SetDesktopFullscreen(fullDesktop);
	}
}
