#pragma once

#include "SubWindow.h"

class WindowHardware : public SubWindow
{
public:
	WindowHardware();
	~WindowHardware() override;

protected:
	void DrawWindowContents() override;

private:
	void GetSoftwareVersionsAndGPUInfo();
	void GetSoftwareVersions();
	void GetCPUinfo();
	void GetCaps();
	void GetGPUinfo();
	void UpdateAvailableMemory();

	std::string FormatVersion(unsigned int major, unsigned int minor, unsigned int patch) const;

	bool versionsSet;
	std::string glewVersion;
	std::string openGLVersion;
	std::string glslVersion;
	std::string sdlVersion;
	std::string assimpVersion;
	std::string dirextXTexVersion;
	std::string imguiVersion;
	std::string mathGeoLibVersion;

	std::string cpusAndCache;
	char caps[75]{};

	std::string ram;
	std::string gpuVendor;
	std::string gpuBrand;

	std::string availableRam;
	std::string totalRam;
	std::string usedRam;
	std::string reservedRam;
};

