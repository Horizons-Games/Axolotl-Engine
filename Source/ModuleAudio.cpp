#include "ModuleAudio.h"

#include <assert.h>

#include <AK/SoundEngine/Common/AkMemoryMgr.h> // Memory Manager interface
#include <AK/SoundEngine/Common/AkModule.h> // Default memory manager
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/SoundEngine/Common/AkSoundEngine.h> // Sound engine
#include <AK/MusicEngine/Common/AkMusicEngine.h> // Music Engine
#include <AK/SpatialAudio/Common/AkSpatialAudio.h> // Spatial Audio
#include <AK/Tools/Common/AkPlatformFuncs.h> // Thread defines



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

    // Customize the Stream Manager settings here.


    if (!AK::StreamMgr::Create(stmSettings))
    {
        assert(!"Could not create the Streaming Manager");
        return false;
    }
    // Create a streaming device with blocking low-level I/O handshaking.
    // Note that you can override the default low-level I/O module with your own. 

    AkDeviceSettings deviceSettings;
    AK::StreamMgr::GetDefaultDeviceSettings(deviceSettings);

    // Customize the streaming device settings here.
    
    // CAkFilePackageLowLevelIOBlocking::Init() creates a streaming device
    // in the Stream Manager, and registers itself as the File Location Resolver.

    if (g_lowLevelIO.Init(deviceSettings) != AK_Success)
    {
        assert(!"Could not create the streaming device and Low-Level I/O system");
        return false;
    }


    return false;
}

bool ModuleAudio::Start()
{
    return false;
}

bool ModuleAudio::CleanUp()
{
    return false;
}

update_status ModuleAudio::PreUpdate()
{
    return update_status();
}

update_status ModuleAudio::Update()
{
    return update_status();
}

update_status ModuleAudio::PostUpdate()
{
    return update_status();
}
