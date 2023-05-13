/*******************************************************************************
The content of this file includes portions of the AUDIOKINETIC Wwise Technology
released in source code form as part of the SDK installer package.

Commercial License Usage

Licensees holding valid commercial licenses to the AUDIOKINETIC Wwise Technology
may use this file in accordance with the end user license agreement provided 
with the software or, alternatively, in accordance with the terms contained in a
written agreement between you and Audiokinetic Inc.

  Copyright (c) 2023 Audiokinetic Inc.
*******************************************************************************/

#ifndef __AK_JOBWORKERMGR_DEFAULT_H__
#define __AK_JOBWORKERMGR_DEFAULT_H__

#include <AK/SoundEngine/Common/AkSoundEngine.h>

namespace AK
{
	namespace JobWorkerMgr
	{
		struct InitSettings
		{
			InitSettings();

			AkJobMgrSettings GetJobMgrSettings();

			AkUInt32 uExecutionTimeUSec; // Maximum amount of time allotted for one execution of a worker, in microseconds. Defaults to 0 (no timeout).
			AkUInt32 uNumWorkerThreads;  // How many threads to allocate for processing jobs. Defaults to 2
			AkThreadProperties* arThreadWorkerProperties; // Array of thread settings, should have uNumWorkerThreads elements. If null, will take default thread settings
		};
		void GetDefaultInitSettings(InitSettings& out_initSettings);

		// Checks whether InitWorkers has been called
		bool IsInitialized();

		// Initialize all of the worker threads.
		AKRESULT InitWorkers(const InitSettings& in_implInitSettings);

		// Terminate all of the worker threads and clean things up
		void TermWorkers();
	}
}

#endif // __AK_JOBWORKERMGR_DEFAULT_H__
