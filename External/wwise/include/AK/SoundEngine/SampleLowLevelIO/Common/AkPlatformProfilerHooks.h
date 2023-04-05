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

#pragma once

#include <AK/SoundEngine/Common/AkTypes.h>

// Functions to be implemented for sample implementations of platform-specific hooks into external profiling tools
void AkPlatformProfilerPushTimer(AkPluginID in_uPluginID, const char* in_pszZoneName);
void AkPlatformProfilerPopTimer();
void AkPlatformProfilerPostMarker(AkPluginID in_uPluginID, const char* in_pszMarkerName);

struct AkInitSettings;
void AkSetPlatformProfilerHooks(AkInitSettings& initSettings);
void AkClearPlatformProfilerHooks(AkInitSettings& initSettings);
