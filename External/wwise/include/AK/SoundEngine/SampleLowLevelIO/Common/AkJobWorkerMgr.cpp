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

#include "AK/SoundEngine/SampleLowLevelIO/Win32/stdafx.h"
#include "AkJobWorkerMgr.h"
#include <AK/Tools/Common/AkFifoQueue.h>
#include <AK/Tools/Common/AkInstrument.h>

#define AK_JOBMGR_DEBUGMSG(...) // AKPLATFORM::OutputDebugMsgV(__VA_ARGS__)

#if defined(AK_SUPPORT_THREADS)
#define AK_JOBWORKERMGR_DEFAULT_NUM_THREADS 2
#else
#define AK_JOBWORKERMGR_DEFAULT_NUM_THREADS 0
#endif

namespace AK
{
namespace JobWorkerMgr
{
	typedef AkFifoQueue<AkJobType, AkJobType_Generic, ArrayPoolLEngineDefault> JobTypeQueue;
	struct WorkerState
	{
		AkThread workerThread;
		AkUInt32 uExecutionTimeUSec;
		AkUInt32 uWorkerIndex;
		volatile bool bKeepThreadAlive;
	};
	WorkerState* g_arWorkerThreadStates = nullptr;
	AkSemaphore g_semaphore;
	AkJobWorkerFunc g_fnJobWorker = nullptr;
	InitSettings g_settings;
	JobTypeQueue g_jobTypeQueue;
	bool g_bIsInitialized = false;

	// Example function that should be provided by game runtime to signal execution of a job.
	// This signals to one of the worker threads that a job is available for execution, and should be woken up
	void AKSOUNDENGINE_CALL RequestJobWorker(AkJobWorkerFunc in_fnJobWorker, AkJobType in_jobType, AkUInt32 in_uNumWorkers, void* in_pClientData)
	{
		g_fnJobWorker = in_fnJobWorker;
		for (int i = 0; i < (int)in_uNumWorkers; i++)
		{
			while (!g_jobTypeQueue.Enqueue(in_jobType))
			{
				// if the job could not be enqueued, then yield the thread so that someone else can clear up the queue, and try again
				AkThreadYield();
			}
		}
		AKPLATFORM::AkReleaseSemaphore(g_semaphore, in_uNumWorkers);
	}

	AK_DECLARE_THREAD_ROUTINE(JobWorkerThread)
	{
		WorkerState& workerState = *AK_GET_THREAD_ROUTINE_PARAMETER_PTR(WorkerState);

		AK_INSTRUMENT_THREAD_START("AK::JobMgr::WorkerThread");
		AK::MemoryMgr::InitForThread();

		// Sleep until signaled the first time
		AK_JOBMGR_DEBUGMSG("Worker #%d going to sleep\n", workerState.uWorkerIndex);
		AKPLATFORM::AkWaitForSemaphore(g_semaphore);
		AK_JOBMGR_DEBUGMSG("Worker #%d Waking up for work\n", workerState.uWorkerIndex);

		if (g_fnJobWorker != nullptr)
		{
			while (workerState.bKeepThreadAlive)
			{
				AkJobType jobType;
				{
					AkInt64 endSpinTime = 0;
					while (!g_jobTypeQueue.Dequeue(jobType))
					{
						// if the dequeue failed, just try again in a light spin. There may be multiple enqueues happening,
						// and we were woken up by a later enqueue, and another enqueue is not yet done its writing.
						if (endSpinTime == 0)
						{
							AKPLATFORM::PerformanceCounter(&endSpinTime);
							endSpinTime += AkInt64(AK::g_fFreqRatio * 0.01); // only spin for about 10us
						}

						// wait a bit or then yield to another thread
						AkInt64 currentTime = 0;
						AKPLATFORM::PerformanceCounter(&currentTime);
						if (currentTime < endSpinTime)
						{
							AkSpinHint();
						}
						else
						{
							AkThreadYield();
						}
					}
				}

				g_fnJobWorker(jobType, workerState.uExecutionTimeUSec);

				// Going back to sleep, so release any thread-local memory
				AK::MemoryMgr::TrimForThread();

				// Go back to sleep until signaled for new work
				AK_JOBMGR_DEBUGMSG("Worker #%d going to sleep\n", workerState.uWorkerIndex);
				AKPLATFORM::AkWaitForSemaphore(g_semaphore);
				AK_JOBMGR_DEBUGMSG("Worker #%d Waking up for work\n", workerState.uWorkerIndex);
			}
		}

		AK::MemoryMgr::TermForThread();
		AkExitThread(AK_RETURN_THREAD_OK);
	}

	InitSettings::InitSettings()
		: uExecutionTimeUSec(0)
		, uNumWorkerThreads(AK_JOBWORKERMGR_DEFAULT_NUM_THREADS)
		, arThreadWorkerProperties(nullptr)
	{
	}

	AkJobMgrSettings InitSettings::GetJobMgrSettings()
	{
		AkJobMgrSettings jobMgrSettings;
		jobMgrSettings.fnRequestJobWorker = uNumWorkerThreads > 0 ? RequestJobWorker : nullptr;
		for (int i = 0; i < AK_NUM_JOB_TYPES; i++)
		{
			jobMgrSettings.uMaxActiveWorkers[i] = uNumWorkerThreads;
		}
		jobMgrSettings.uMemorySlabSize = 8 * 1024;
		jobMgrSettings.uNumMemorySlabs = uNumWorkerThreads + 1; // One slab per thread (AudioMgr + worker threads) is a good initial value
		return jobMgrSettings;
	}

	void GetDefaultInitSettings(InitSettings& out_initSettings)
	{
		out_initSettings = InitSettings();
	}

	bool IsInitialized()
	{
		return g_bIsInitialized;
	}

	// Initialize all of the worker threads and bookkeeping variables
	AKRESULT InitWorkers(const InitSettings& in_implInitSettings)
	{
		if (in_implInitSettings.uNumWorkerThreads == 0)
			return AK_InvalidParameter;

		AKRESULT ret = AK_Success;

		// Entries in the queue must be a power of 2
		AkUInt32 uNumEntries = 2;
		while (uNumEntries < in_implInitSettings.uNumWorkerThreads * AK_NUM_JOB_TYPES)
		{
			uNumEntries <<= 1;
		}
		ret = g_jobTypeQueue.Init(uNumEntries);
		if (ret != AK_Success)
			return ret;

		AkUInt32 uNumWorkerThreads = in_implInitSettings.uNumWorkerThreads;
		g_arWorkerThreadStates = (WorkerState*)AK::MemoryMgr::Malloc(AkMemID_Processing, uNumWorkerThreads * sizeof(WorkerState));
		if (g_arWorkerThreadStates == NULL)
		{
			return AK_InsufficientMemory;
		}
		AKPLATFORM::AkMemSet(g_arWorkerThreadStates, 0, uNumWorkerThreads * sizeof(WorkerState));

		g_settings = in_implInitSettings;

		ret = AKPLATFORM::AkCreateSemaphore(g_semaphore, 0);
		if (ret != AK_Success)
		{
			return ret;
		}

		AkThreadProperties defaultThreadProps;
		AKPLATFORM::AkGetDefaultThreadProperties(defaultThreadProps);
		WorkerState* pNextWorkerState = g_arWorkerThreadStates;
		for (AkUInt32 workerIdx = 0; workerIdx < uNumWorkerThreads; ++workerIdx)
		{
			pNextWorkerState->uExecutionTimeUSec = in_implInitSettings.uExecutionTimeUSec;
			pNextWorkerState->bKeepThreadAlive = true;
			pNextWorkerState->uWorkerIndex = workerIdx;

			char threadName[64];
			snprintf(threadName, 64, "AK::JobMgr::WorkerThread-%d", workerIdx);
			AKPLATFORM::AkCreateThread(
				JobWorkerThread,
				pNextWorkerState,
				in_implInitSettings.arThreadWorkerProperties ? in_implInitSettings.arThreadWorkerProperties[workerIdx] : defaultThreadProps,
				&pNextWorkerState->workerThread,
				threadName);

			if (!AKPLATFORM::AkIsValidThread(&pNextWorkerState->workerThread))
			{
				return AK_Fail;
			}
			pNextWorkerState++;
		}
		g_bIsInitialized = true;
		return AK_Success;
	}

	// Terminate all of the worker threads and clean things up
	void TermWorkers()
	{
		if (g_arWorkerThreadStates)
		{
			// Mark all of the threads as inactive
			for (AkUInt32 i = 0; i < g_settings.uNumWorkerThreads; ++i)
			{
				g_arWorkerThreadStates[i].bKeepThreadAlive = false;
			}

			// signal all of the them to wake-up to process said inactivity
			AKPLATFORM::AkReleaseSemaphore(g_semaphore, g_settings.uNumWorkerThreads);

			// and then wait for them to gracefully close
			for (AkUInt32 i = 0; i < g_settings.uNumWorkerThreads; ++i)
			{
				AKPLATFORM::AkWaitForSingleThread(&g_arWorkerThreadStates[i].workerThread);
				AKPLATFORM::AkCloseThread(&g_arWorkerThreadStates[i].workerThread);
			}

			AKPLATFORM::AkDestroySemaphore(g_semaphore);
			AKPLATFORM::AkClearSemaphore(g_semaphore);
			AK::MemoryMgr::Free(AkMemID_Processing, g_arWorkerThreadStates);
		}

		g_jobTypeQueue.Term();
		g_bIsInitialized = false;
	}

} // AK::JobWorkerMgr
} // AK
