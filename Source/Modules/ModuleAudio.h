#pragma once
#include "Module.h"
#include "AK/SoundEngine/SampleLowLevelIO/Win32/AkFilePackageLowLevelIOBlocking.h"

namespace audios 
{
	static const wchar_t* SOUND_LASER_FIRE = L"SFX_Laser_Fire";
};

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