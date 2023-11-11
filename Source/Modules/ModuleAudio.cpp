#include "StdAfx.h"

#include "ModuleAudio.h"

#include "Application.h"
#include "FileSystem/ModuleFileSystem.h"
#include "Auxiliar/Audio/AudioData.h"

#include "AK/MusicEngine/Common/AkMusicEngine.h" // Music Engine
#include "AK/SoundEngine/Common/AkMemoryMgr.h"	 // Memory Manager interface
#include "AK/SoundEngine/Common/AkModule.h"		 // Default memory manager
#include "AK/SoundEngine/Common/AkSoundEngine.h" // Sound engine
#include "AK/SoundEngine/Common/AkStreamMgrModule.h"
#include "AK/SpatialAudio/Common/AkSpatialAudio.h" // Spatial Audio
#include "AK/Tools/Common/AkPlatformFuncs.h"	   // Thread defines

#ifndef AK_OPTIMIZED
	#include "AK/Comm/AkCommunication.h"
#endif // AK_OPTIMIZED

#define BANKNAME_INIT L"Init.bnk"

ModuleAudio::ModuleAudio()
{
}

ModuleAudio::~ModuleAudio()
{
}

bool ModuleAudio::Init()
{
	AkMemSettings memSettings;
	AK::MemoryMgr::GetDefaultSettings(memSettings);

	if (AK::MemoryMgr::Init(&memSettings) != AK_Success)
	{
		assert(!"Could not create the memory manager.");
		return false;
	}

	AkStreamMgrSettings stmSettings;
	AK::StreamMgr::GetDefaultSettings(stmSettings);

	// Customize the Stream Manager settings

	if (!AK::StreamMgr::Create(stmSettings))
	{
		assert(!"Could not create the Streaming Manager");
		return false;
	}
	// Create a streaming device with blocking low-level I/O handshaking.
	// Note that you can override the default low-level I/O module with your own.

	AkDeviceSettings deviceSettings;
	AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

	// Customize the streaming device settings
	// CAkFilePackageLowLevelIOBlocking::Init() creates a streaming device
	// in the Stream Manager, and registers itself as the File Location Resolver.

	if (lowLevelIO.Init(deviceSettings) != AK_Success)
	{
		assert(!"Could not create the streaming device and Low-Level I/O system");
		return false;
	}

	// Create the Sound Engine
	// Using default initialization parameters

	AkInitSettings initSettings;
	AkPlatformInitSettings platformInitSettings;
	AK::SoundEngine::GetDefaultInitSettings(initSettings);
	AK::SoundEngine::GetDefaultPlatformInitSettings(platformInitSettings);

	if (AK::SoundEngine::Init(&initSettings, &platformInitSettings) != AK_Success)
	{
		assert(!"Could not initialize the Sound Engine.");
		return false;
	}

	// Initialize the music engine
	// Using default initialization parameters

	AkMusicSettings musicInit;
	AK::MusicEngine::GetDefaultInitSettings(musicInit);

	if (AK::MusicEngine::Init(&musicInit) != AK_Success)
	{
		assert(!"Could not initialize the Music Engine.");
		return false;
	}

	// Initialize Spatial Audio
	// Using default initialization parameters

	AkSpatialAudioInitSettings
		settings; // The constructor fills AkSpatialAudioInitSettings with the recommended default settings.

	if (AK::SpatialAudio::Init(settings) != AK_Success)
	{
		assert(!"Could not initialize the Spatial Audio.");
		return false;
	}

#ifndef AK_OPTIMIZED
	// Initialize communications (not in release build!)

	AkCommSettings commSettings;
	AK::Comm::GetDefaultInitSettings(commSettings);
	if (AK::Comm::Init(commSettings) != AK_Success)
	{
		assert(!"Could not initialize communication.");
		return false;
	}
#endif // AK_OPTIMIZED

	return InitializeBanks();
}

bool ModuleAudio::Start()
{
	SetLowPassFilter(0.0f);
	return true;
}

bool ModuleAudio::CleanUp()
{
	AK::SoundEngine::StopAll();
	AK::SoundEngine::UnregisterAllGameObj();
	AK::SoundEngine::RenderAudio();

	// Try to prevent data race
	// This is very far from ideal
	// We should wait for RenderAudio to end
	// But I couldn't figure out a way
	AKPLATFORM::AkSleep(10);

#ifndef AK_OPTIMIZED
	// Terminate Communication Services
	AK::Comm::Term();
#endif // AK_OPTIMIZED

	// Terminate Spatial Audio
	// AK::SpatialAudio::Term();

	// Terminate the music engine
	AK::MusicEngine::Term();

	// Terminate the sound engine
	AK::SoundEngine::Term();

	// Terminate the streaming device and streaming manager
	// CAkFilePackageLowLevelIOBlocking::Term() destroys its associated streaming device
	// that lives in the Stream Manager, and unregisters itself as the File Location Resolver.
	lowLevelIO.Term();
	if (AK::IAkStreamMgr::Get())
	{
		AK::IAkStreamMgr::Get()->Destroy();
	}

	// Terminate the Memory Manager
	AK::MemoryMgr::Term();

	return true;
}

UpdateStatus ModuleAudio::PreUpdate()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleAudio::Update()
{
	// Process bank requests, events, positions, RTPC, etc.
	AK::SoundEngine::RenderAudio();

	return UpdateStatus::UPDATE_CONTINUE;
}

UpdateStatus ModuleAudio::PostUpdate()
{
	return UpdateStatus::UPDATE_CONTINUE;
}

void ModuleAudio::Suspend() const
{
	AK::SoundEngine::Suspend(false, false);
}

void ModuleAudio::WakeUp() const
{
	AK::SoundEngine::WakeupFromSuspend();
}

bool ModuleAudio::InitializeBanks()
{
	// Setup banks path
	std::string soundBanksFolderPath;

	{
		const wchar_t* pathAsWChar = AKTEXT("WwiseProject/GeneratedSoundBanks/Windows");
		lowLevelIO.SetBasePath(pathAsWChar);
		std::wstring pathAsWString = std::wstring(pathAsWChar);
		std::transform(std::begin(pathAsWString),
					   std::end(pathAsWString),
					   std::back_inserter(soundBanksFolderPath),
					   [](wchar_t c)
					   {
						   return static_cast<char>(c);
					   });
	}

	AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));

	// Load banks synchronously (from file name).
	AkBankID bankID; // Not used. These banks can be unloaded with their file name.
	ModuleFileSystem* fileSystem = App->GetModule<ModuleFileSystem>();

	std::vector<std::string> bankFolderContents = fileSystem->ListFiles(soundBanksFolderPath.c_str());

	{
		AKRESULT eResult = AK::SoundEngine::LoadBank(BANKNAME_INIT, bankID);
		if (eResult != AK_Success)
		{
			LOG_ERROR("Failed to load bank {}; Error: {}", BANKNAME_INIT, eResult);
			return false;
		}
	}

	for (const std::string& bankFolderItem : bankFolderContents)
	{
		if (bankFolderItem != "Init.bnk" &&
			App->GetModule<ModuleFileSystem>()->GetFileExtension(bankFolderItem) == ".bnk")
		{
			std::wstring bankNameAsWString = std::wstring(std::begin(bankFolderItem), std::end(bankFolderItem));
			AKRESULT eResult = AK::SoundEngine::LoadBank(bankNameAsWString.c_str(), bankID);
			if (eResult != AK_Success)
			{
				LOG_ERROR("Failed to load bank {}; Error: {}", bankFolderItem, eResult);
				return false;
			}
		}
	}
	
	return true;
}

void ModuleAudio::SetMasterVolume(float value)
{
	AK::SoundEngine::SetRTPCValue(AUDIO::CONTROLLERS::MASTERVOLUME, value);
}

void ModuleAudio::SetMusicVolume(float value)
{
	AK::SoundEngine::SetRTPCValue(AUDIO::CONTROLLERS::MUSICVOLUME, value);
}

void ModuleAudio::SetSFXVolume(float value)
{
	AK::SoundEngine::SetRTPCValue(AUDIO::CONTROLLERS::SFXVOLUME, value);
}

void ModuleAudio::SetLowPassFilter(float value)
{
	AK::SoundEngine::SetRTPCValue(AUDIO::CONTROLLERS::LOWPASSFILTER, value);
}

void ModuleAudio::SetMusicID(uint64_t sourceID)
{
	musicID = sourceID;
}

void ModuleAudio::SetMusicSwitch(const wchar_t* switchGroup, const wchar_t* switchSound)
{
	AK::SoundEngine::SetSwitch(switchGroup, switchSound, musicID);
}