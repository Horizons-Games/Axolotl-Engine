#include "ModuleAudio.h"

#include <assert.h>

#include <AK/SoundEngine/Common/AkMemoryMgr.h> // Memory Manager interface
#include <AK/SoundEngine/Common/AkModule.h> // Default memory manager
#include <AK/SoundEngine/Common/IAkStreamMgr.h> // Streaming Manager
#include <AK/Tools/Common/AkPlatformFuncs.h> // Thread defines
#include <AK/SoundEngine/POSIX/AkFilePackageLowLevelIOBlocking.h> // Sample low-level I/O implementation


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

    //

    // Create and initialize an instance of the default streaming manager. Note

    // that you can override the default streaming manager with your own. 

    //

	AkStreamMgrSettings stmSettings;
    

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
