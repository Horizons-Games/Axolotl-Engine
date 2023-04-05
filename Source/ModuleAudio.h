#pragma once
#pragma once
#include "Module.h"
#include <AK/SoundEngine/SampleLowLevelIO/POSIX/AkFilePackageLowLevelIOBlocking.h>

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
	CAkFilePackageLowLevelIOBlocking g_lowLevelIO;
};