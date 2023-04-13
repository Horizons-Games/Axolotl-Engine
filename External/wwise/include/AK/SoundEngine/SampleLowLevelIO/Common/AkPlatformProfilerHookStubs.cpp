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

// functions are intentionally left undefined
// void AkPlatformProfilerPushTimer(AkPluginID in_uPluginID, const char* in_pszZoneName)
// {
// }

// void AkPlatformProfilerPopTimer()
// {
// }

// void AkPlatformProfilerPostMarker(AkPluginID in_uPluginID, const char* in_pszMarkerName)
// {
// }

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