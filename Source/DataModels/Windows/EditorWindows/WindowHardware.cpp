#include "WindowHardware.h"

#include <cmath>

#include "imgui.h"
#include "GL/glew.h"
#include "SDL.h"
#include "assimp/version.h"
#include "d3d9.h"

WindowHardware::WindowHardware() : EditorWindow("Hardware")
{
	flags |= ImGuiWindowFlags_AlwaysAutoResize;

	GetCPUinfo();
	GetCaps();
	GetGPUinfo();
}

WindowHardware::~WindowHardware()
{
}

void WindowHardware::DrawWindowContents()
{
	GetSoftwareVersions();

	ImGui::TextUnformatted(("glew Version: " + glewVersion).c_str());
	ImGui::TextUnformatted(("OpenGL Version: " + openGLVersion).c_str());
	ImGui::TextUnformatted(("SDL Version: " + sdlVersion).c_str());
	ImGui::TextUnformatted(("assimp Version: " + assimpVersion).c_str());
	ImGui::TextUnformatted(("DirectXTex Version: " + dirextXTexVersion).c_str());
	ImGui::TextUnformatted(("ImGui Version: " + imguiVersion).c_str());
	ImGui::TextUnformatted(("MathGeoLib Version: " + mathGeoLibVersion).c_str());

	ImGui::Separator();

	ImGui::TextUnformatted(("CPUs: " + cpusAndCache).c_str());
	ImGui::TextUnformatted(("System RAM: " + ram).c_str());
	ImGui::TextUnformatted("Caps: ");
	ImGui::SameLine();
	ImGui::TextWrapped(caps.c_str());

	ImGui::Separator();

	ImGui::TextUnformatted(("GPU: " + gpuVendorAndDevice).c_str());
	ImGui::TextUnformatted(("Brand: " + gpuBrand).c_str());

	UpdateAvailableMemory();
	ImGui::TextUnformatted(("VRAM Available: " + availableRam + "Mb").c_str());
	ImGui::TextUnformatted(("VRAM Total: " + totalRam + "Mb").c_str());
	ImGui::TextUnformatted(("VRAM Used: " + usedRam + "Mb").c_str());
}

void WindowHardware::GetSoftwareVersions()
{
	//this needs to be done after all modules have been initialized
	//this way is a bit ugly but otherwise it would require a Start/Init function in Window that would be useless for the others
	if (!versionsSet)
	{
		versionsSet = true;

		char glewVersionBuf[128];
		sprintf(glewVersionBuf, "%s", glewGetString(GLEW_VERSION));
		glewVersion = std::string(glewVersionBuf);

		char glSupported[128];
		sprintf(glSupported, "%s", glGetString(GL_VERSION));
		openGLVersion = std::string(glSupported);

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
	int cacheSizeInKB = std::ceil(cacheSizeInB / 1000.f);
	int cacheSizeInKb = cacheSizeInKB * 8;
	cpusAndCache = std::to_string(SDL_GetCPUCount()) + " (Cache: " + std::to_string(cacheSizeInKb) + "kb)";
}

void WindowHardware::GetCaps()
{
	caps = "";
	if (SDL_Has3DNow())
		caps += "3DNow ";
	if (SDL_HasAltiVec())
		caps += "AltiVec ";
	if (SDL_HasAVX())
		caps += "AVX ";
	if (SDL_HasAVX2())
		caps += "AVX2 ";
	if (SDL_HasMMX())
		caps += "MMX ";
	if (SDL_HasRDTSC())
		caps += "RDTSC ";
	if (SDL_HasSSE())
		caps += "SSE ";
	if (SDL_HasSSE2())
		caps += "SSE2 ";
	if (SDL_HasSSE3())
		caps += "SSE3 ";
	if (SDL_HasSSE41())
		caps += "SSE41 ";
	if (SDL_HasSSE42())
		caps += "SSE42 ";
}

void WindowHardware::GetGPUinfo()
{
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

	/*LPDIRECT3D9 pD3D = nullptr;
	pD3D = Direct3DCreate9(D3D_SDK_VERSION);
	D3DADAPTER_IDENTIFIER9 adapterIdentifier;
	pD3D->GetAdapterIdentifier(D3DADAPTER_DEFAULT, 0, &adapterIdentifier);

	std::string gpuVendor = std::to_string(adapterIdentifier.VendorId);
	std::string gpuDevice = std::to_string(adapterIdentifier.DeviceId);
	gpuVendorAndDevice = "vendor " + gpuVendor + " device " + gpuDevice;
	gpuBrand = adapterIdentifier.Description;*/
}

void WindowHardware::UpdateAvailableMemory()
{
	int availableRamKb = 0;
	int totalRamKb = 0;
	glGetIntegerv(GL_GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &availableRamKb);
	glGetIntegerv(GL_GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &totalRamKb);

	float availableRamMb = availableRamKb / 1000.f;
	float totalRamMb = totalRamKb / 1000.f;
	float usedRamMb = totalRamMb - availableRamMb;

	char buf[128];
	sprintf_s(buf, 128, "%.2f", availableRamMb);
	availableRam = std::string(buf);
	sprintf_s(buf, 128, "%.2f", totalRamMb);
	totalRam = std::string(buf);
	sprintf_s(buf, 128, "%.2f", usedRamMb);
	usedRam = std::string(buf);
}

std::string WindowHardware::FormatVersion(unsigned int major, unsigned int minor, unsigned int patch) const
{
	return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);;
}
