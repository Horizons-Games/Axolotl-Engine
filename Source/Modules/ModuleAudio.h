#pragma once
#include "AK/SoundEngine/SampleLowLevelIO/Win32/AkFilePackageLowLevelIOBlocking.h"
#include "Module.h"

class ModuleAudio : public Module
{
public:
	ModuleAudio();
	~ModuleAudio() override;

	bool Init() override;
	bool Start() override;

	UpdateStatus PreUpdate() override;
	UpdateStatus Update() override;
	UpdateStatus PostUpdate() override;

	bool CleanUp() override;

private:
	bool InitializeBanks();

	CAkFilePackageLowLevelIOBlocking lowLevelIO;
};