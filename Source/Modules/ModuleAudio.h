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
	bool CleanUp() override;

	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();

private:
	void InitializeBanks();

	CAkFilePackageLowLevelIOBlocking lowLevelIO;
};