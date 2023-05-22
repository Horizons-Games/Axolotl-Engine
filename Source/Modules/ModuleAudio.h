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

	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;

	bool CleanUp() override;

private:
	bool InitializeBanks();

	CAkFilePackageLowLevelIOBlocking lowLevelIO;
};