#include "ModuleAudio.h"

#include <assert.h>

#include "AK/SoundEngine/Common/AkMemoryMgr.h" // Memory Manager interface
#include "AK/SoundEngine/Common/AkModule.h" // Default memory manager
#include "AK/SoundEngine/Common/AkStreamMgrModule.h"
#include "AK/SoundEngine/Common/AkSoundEngine.h" // Sound engine
#include "AK/MusicEngine/Common/AkMusicEngine.h" // Music Engine
#include "AK/SpatialAudio/Common/AkSpatialAudio.h" // Spatial Audio
#include "AK/Tools/Common/AkPlatformFuncs.h" // Thread defines

#ifndef AK_OPTIMIZED
#include "AK/Comm/AkCommunication.h"
#endif // AK_OPTIMIZED

// Bank file names
#define BANKNAME_INIT L"Init.bnk"
#define BANKNAME_MAIN L"Main.bnk"


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

    AkSpatialAudioInitSettings settings; // The constructor fills AkSpatialAudioInitSettings with the recommended default settings. 

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

    InitializeBanks();

    return true;
}

bool ModuleAudio::Start()
{
    return true;
}

bool ModuleAudio::CleanUp()
{
    AK::SoundEngine::UnregisterAllGameObj();

#ifndef AK_OPTIMIZED
    // Terminate Communication Services
    AK::Comm::Term();
#endif // AK_OPTIMIZED

    // Terminate Spatial Audio
    //AK::SpatialAudio::Term();

    // Terminate the music engine
    AK::MusicEngine::Term();

    // Terminate the sound engine
    AK::SoundEngine::Term();

    // Terminate the streaming device and streaming manager
    // CAkFilePackageLowLevelIOBlocking::Term() destroys its associated streaming device
    // that lives in the Stream Manager, and unregisters itself as the File Location Resolver.
    lowLevelIO.Term();
    if (AK::IAkStreamMgr::Get())
        AK::IAkStreamMgr::Get()->Destroy();

    // Terminate the Memory Manager
    AK::MemoryMgr::Term();

    return true;
}

update_status ModuleAudio::PreUpdate()
{
    return update_status::UPDATE_CONTINUE;
}

update_status ModuleAudio::Update()
{
    // Process bank requests, events, positions, RTPC, etc.
    AK::SoundEngine::RenderAudio();

    return update_status::UPDATE_CONTINUE;
}

update_status ModuleAudio::PostUpdate()
{
    return update_status::UPDATE_CONTINUE;
}

void ModuleAudio::InitializeBanks() {
    // Setup banks path
//

    lowLevelIO.SetBasePath(AKTEXT("WwiseProject/GeneratedSoundBanks/Windows"));

    AK::StreamMgr::SetCurrentLanguage(AKTEXT("English(US)"));

    //// Load banks synchronously (from file name).
    AkBankID bankID; // Not used. These banks can be unloaded with their file name.


    AKRESULT eResult = AK::SoundEngine::LoadBank(BANKNAME_INIT, bankID);
    assert(eResult == AK_Success);
    eResult = AK::SoundEngine::LoadBank(BANKNAME_MAIN, bankID);
    assert(eResult == AK_Success);
}
