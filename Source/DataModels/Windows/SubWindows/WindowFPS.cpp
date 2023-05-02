#include "WindowFPS.h"

#include "Application.h"
#include "Modules/ModuleWindow.h"

WindowFPS::WindowFPS() : SubWindow("FPS"), 
	fpsCaptures(100), timeCaptures(100), currentFpsIndex(0), currentTimeIndex(0), 
	fpsHist(std::vector<float>(fpsCaptures)), timeHist(std::vector<float>(timeCaptures)) //all values will be zero
{
}

WindowFPS::~WindowFPS()
{
}

void WindowFPS::DrawWindowContents()
{
	int maxFPS = App->GetMaxFrameRate();
	ImGui::SliderInt("Max FPS", &maxFPS, 1, 120, "%d", ImGuiSliderFlags_AlwaysClamp);
	App->SetMaxFrameRate(maxFPS);
		
	float deltaTime = App->GetDeltaTime();
	fpsHist[currentFpsIndex] = 1.f / deltaTime;
	timeHist[currentTimeIndex] = deltaTime * 1000.f;
		
	char currentFramerate[25]{};
	sprintf_s(currentFramerate, 25, "Framerate %.1f", fpsHist[currentFpsIndex]);
	ImGui::PlotHistogram("##framerate", &fpsHist[0], fpsCaptures, 0, currentFramerate, 0.f, 160.f, ImVec2(310, 160));
	
	char currentTime[25]{};
	sprintf_s(currentTime, 25, "Milliseconds %.1f", timeHist[currentTimeIndex]);
	ImGui::PlotHistogram("##milliseconds", &timeHist[0], timeCaptures, 0, currentTime, 0.0f, 40.0f, ImVec2(310, 100));
		
	if (currentFpsIndex < fpsCaptures - 1) 
	{
		++currentFpsIndex;
	}
	else 
	{
		//remove the first element and increase the size of the vector back
		fpsHist.erase(fpsHist.begin());
		fpsHist.push_back(0);
	}

	if (currentTimeIndex < timeCaptures - 1) 
	{
		++currentTimeIndex;
	}
	else 
	{
		//remove the first element and increase the size of the vector back
		timeHist.erase(timeHist.begin());
		timeHist.push_back(0);
	}
	
	bool vsyncActive = App->GetModule<ModuleWindow>()->GetVsync();
	if (ImGui::Checkbox("Vsync", &vsyncActive))
	{
		App->GetModule<ModuleWindow>()->SetVsync(vsyncActive);
	}

}
