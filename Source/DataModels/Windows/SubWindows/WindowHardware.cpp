#include "WindowHardware.h"

#include <cmath>

#include "imgui.h"
#include "GL/glew.h"
#include "SDL.h"
#include "assimp/version.h"

WindowHardware::WindowHardware() : SubWindow("Hardware")
{
	GetCPUinfo();
	GetCaps();
}

WindowHardware::~WindowHardware()
{
}

void WindowHardware::DrawWindowContents()
{
	GetSoftwareVersionsAndGPUInfo();

	ImGui::Text("glew Version: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), glewVersion.c_str());
	
	ImGui::Text("OpenGL Version: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), openGLVersion.c_str());

	ImGui::Text("GLSL: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), glslVersion.c_str());

	ImGui::Text("SDL Version: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), sdlVersion.c_str());

	ImGui::Text("assimp Version: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), assimpVersion.c_str());

	ImGui::Text("DirectXTex Version: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), dirextXTexVersion.c_str());

	ImGui::Text("ImGui Version: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), imguiVersion.c_str());

	ImGui::Text("MathGeoLib Version: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), mathGeoLibVersion.c_str());

	ImGui::Separator();

	ImGui::Text("CPUs: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), cpusAndCache.c_str());

	ImGui::Text("System RAM: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), ram.c_str());

	ImGui::Text("Caps: ");
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f));
	ImGui::TextWrapped(caps);
	ImGui::PopStyleColor(1);

	ImGui::Separator();

	ImGui::TextUnformatted("GPU: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), gpuVendor.c_str());

	ImGui::TextUnformatted("Brand: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), gpuBrand.c_str());

	UpdateAvailableMemory();
	ImGui::TextUnformatted("VRAM Available: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), (availableRam + "Mb").c_str());

	ImGui::TextUnformatted("VRAM Total: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), (totalRam + "Mb").c_str());

	ImGui::TextUnformatted("VRAM Used: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), (usedRam + "Mb").c_str());

	ImGui::TextUnformatted("VRAM Reserved: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), (reservedRam + "Mb").c_str());
}

void WindowHardware::GetSoftwareVersionsAndGPUInfo()
{
	//this needs to be done after all modules have been initialized
	//this way is a bit ugly but otherwise it would require a Start/Init function in Window that would be useless for the others
	if (!versionsSet)
	{
		GetGPUinfo();
		GetSoftwareVersions();

		versionsSet = true;
	}
}

void WindowHardware::GetSoftwareVersions()
{
	if (!versionsSet)
	{
		char glewVersionBuf[128]{};
		sprintf(glewVersionBuf, "%s", glewGetString(GLEW_VERSION));
		glewVersion = std::string(glewVersionBuf);

		char glSupported[128]{};
		sprintf(glSupported, "%s", glGetString(GL_VERSION));
		openGLVersion = std::string(glSupported);

		char glsl[128]{};
		sprintf(glsl, "%s", glGetString(GL_SHADING_LANGUAGE_VERSION));
		glslVersion = std::string(glsl);

		sdlVersion = FormatVersion(SDL_MAJOR_VERSION, SDL_MINOR_VERSION, SDL_PATCHLEVEL);

		assimpVersion = FormatVersion(aiGetVersionMajor(), aiGetVersionMinor(), aiGetVersionRevision());

		dirextXTexVersion = "1.9.7";

		mathGeoLibVersion = "2.0";

		imguiVersion = IMGUI_VERSION;
	}
}

void WindowHardware::GetCPUinfo()
{
	int cacheSizeInB = SDL_GetCPUCacheLineSize();
	int cacheSizeInKB = std::ceil(cacheSizeInB / 1024.f);
	int cacheSizeInKb = cacheSizeInKB * 8;
	cpusAndCache = std::to_string(SDL_GetCPUCount()) + " (Cache: " + std::to_string(cacheSizeInKb) + "kb)";

	int ramInMB = SDL_GetSystemRAM();
	float ramInGB = ramInMB / 1000.f;
	float ramInGb = ramInGB * 8.f;
	//this is the value multiplied by 10 to the power of 8
	//insted of to the power of 9, which is what Giga is
	//this is to make rounding easier,
	//since I can't find a way to set decimal precision
	int ramInGbOneDecimalAux = std::ceil(ramInGb * 10.f);
	std::string ramInGbOneDecimal = std::to_string(ramInGbOneDecimalAux);
	//insert a dot in the penultimate position
	ramInGbOneDecimal.insert(ramInGbOneDecimal.length() - 1, ".");
	ram = ramInGbOneDecimal + "Gb";
}

void WindowHardware::GetCaps()
{
	if (SDL_Has3DNow()) sprintf_s(caps, 75, "%s3DNow, ", caps);
	if (SDL_HasAltiVec()) sprintf_s(caps, 75, "%sAltiVec, ", caps);
	if (SDL_HasAVX()) sprintf_s(caps, 75, "%sAVX, ", caps);
	if (SDL_HasAVX2()) sprintf_s(caps, 75, "%sAVX2, ", caps);
	if (SDL_HasMMX()) sprintf_s(caps, 75, "%sMMX, ", caps);
	if (SDL_HasRDTSC()) sprintf_s(caps, 75, "%sRDTSC, ", caps);
	if (SDL_HasSSE()) sprintf_s(caps, 75, "%sSSE, ", caps);
	if (SDL_HasSSE2()) sprintf_s(caps, 75, "%sSSE2, ", caps);
	if (SDL_HasSSE3()) sprintf_s(caps, 75, "%sSSE3, ", caps);
	if (SDL_HasSSE41()) sprintf_s(caps, 75, "%sSSE41, ", caps);
	if (SDL_HasSSE42()) sprintf_s(caps, 75, "%sSSE42, ", caps);
}

void WindowHardware::GetGPUinfo()
{
	if (!versionsSet)
	{
		char gpuVendorBuf[128]{};
		sprintf(gpuVendorBuf, "%s", glGetString(GL_VENDOR));
		gpuVendor = std::string(gpuVendorBuf);

		char gpuBrandBuf[128]{};
		sprintf(gpuBrandBuf, "%s", glGetString(GL_RENDERER));
		gpuBrand = std::string(gpuBrandBuf);
	}
}

void WindowHardware::UpdateAvailableMemory()
{
	int availableRamKb = 0;
	int totalRamKb = 0;
	int reservedRamKb = 0;
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &availableRamKb);
	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalRamKb);
	glGetIntegerv(GL_GPU_MEMORY_INFO_EVICTED_MEMORY_NVX, &reservedRamKb);

	float availableRamMb = availableRamKb / 1024.f;
	float totalRamMb = totalRamKb / 1024.f;
	float usedRamMb = totalRamMb - availableRamMb;
	float reservedRamMb = reservedRamKb / 1024.f;

	char buf[128];
	sprintf_s(buf, 128, "%.2f", availableRamMb);
	availableRam = std::string(buf);
	sprintf_s(buf, 128, "%.2f", totalRamMb);
	totalRam = std::string(buf);
	sprintf_s(buf, 128, "%.2f", usedRamMb);
	usedRam = std::string(buf);
	sprintf_s(buf, 128, "%.2f", reservedRamMb);
	reservedRam = std::string(buf);
}

std::string WindowHardware::FormatVersion(unsigned int major, unsigned int minor, unsigned int patch) const
{
	return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);;
}
