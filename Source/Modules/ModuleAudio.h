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

	void StopAllAudio();

	void SetMasterVolume(float value);
	void SetMusicVolume(float value);
	void SetSFXVolume(float value);
	void SetLowPassFilter(float value);

	void SetMusicID(uint64_t sourceID);
	void SetMusicSwitch(const wchar_t* switchGroup, const wchar_t* switchSound);

private:
	bool InitializeBanks();

	CAkFilePackageLowLevelIOBlocking lowLevelIO;

	uint64_t musicID;
};