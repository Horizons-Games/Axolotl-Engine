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

	void Suspend() const;
	void WakeUp() const;

	void SetMasterVolume(float value);
	void SetMusicVolume(float value);
	void SetSFXVolume(float value);

private:
	bool InitializeBanks();

	CAkFilePackageLowLevelIOBlocking lowLevelIO;
};