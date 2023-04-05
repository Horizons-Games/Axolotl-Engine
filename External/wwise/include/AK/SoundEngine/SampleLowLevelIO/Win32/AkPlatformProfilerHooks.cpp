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

#include "stdafx.h"
#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/Tools/Common/AkProfilingID.h>

// These are sample implementations of the profiler hooks, to tie into the platform-specific profiler functionality.

#if defined(AK_XBOXGC) || defined(AK_XBOXONE)
#define USE_PIX
#include <pix.h>
#pragma comment( lib, "PIXEvt" )

void AkPlatformProfilerPushTimer(AkPluginID in_uPluginID, const char* in_pszZoneName)
{
	PIXBeginEvent(0, in_pszZoneName);
}

void AkPlatformProfilerPopTimer()
{
	PIXEndEvent();
}

void AkPlatformProfilerPostMarker(AkPluginID in_uPluginID, const char* in_pszMarkerName)
{
	if (in_uPluginID != AKMAKECLASSID(AkPluginTypeNone, AKCOMPANYID_AUDIOKINETIC, AK::ProfilingID::AudioFrameBoundary))
	{
		PIXSetMarker(0, in_pszMarkerName);
	}
}

void AkSetPlatformProfilerHooks(AkInitSettings& initSettings)
{
	initSettings.fnProfilerPushTimer = AkPlatformProfilerPushTimer;
	initSettings.fnProfilerPopTimer = AkPlatformProfilerPopTimer;
	initSettings.fnProfilerPostMarker = AkPlatformProfilerPostMarker;
}

void AkClearPlatformProfilerHooks(AkInitSettings& initSettings)
{
	initSettings.fnProfilerPushTimer = nullptr;
	initSettings.fnProfilerPopTimer = nullptr;
	initSettings.fnProfilerPostMarker = nullptr;
}

#else // !( defined(AK_XBOXGC) || defined(AK_XBOXONE) )

void AkSetPlatformProfilerHooks(AkInitSettings& initSettings)
{
	initSettings.fnProfilerPushTimer = nullptr;
	initSettings.fnProfilerPopTimer = nullptr;
	initSettings.fnProfilerPostMarker = nullptr;
}

void AkClearPlatformProfilerHooks(AkInitSettings& initSettings)
{
	initSettings.fnProfilerPushTimer = nullptr;
	initSettings.fnProfilerPopTimer = nullptr;
	initSettings.fnProfilerPostMarker = nullptr;
}

#endif