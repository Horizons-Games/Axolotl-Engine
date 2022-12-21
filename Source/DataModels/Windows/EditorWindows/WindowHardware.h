#pragma once

#include "EditorWindow.h"

#include <string>

class WindowHardware : public EditorWindow
{
public:
	WindowHardware();
	~WindowHardware();

protected:
	void DrawWindowContents() override;

private:
	void GetSoftwareVersions();
	void GetCPUinfo();
	void GetCaps();
	void GetGPUinfo();
	void UpdateAvailableMemory();

	std::string FormatVersion(unsigned int major, unsigned int minor, unsigned int patch) const;

	bool versionsSet = false;
	std::string glewVersion;
	std::string openGLVersion;
	std::string sdlVersion;
	std::string assimpVersion;
	std::string dirextXTexVersion;
	std::string imguiVersion;
	std::string mathGeoLibVersion;

	std::string cpusAndCache;
	std::string caps;

	std::string ram;
	std::string gpuVendorAndDevice;
	std::string gpuBrand;

	std::string availableRam;
	std::string totalRam;
	std::string usedRam;
};

