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
//////////////////////////////////////////////////////////////////////
//
// AkFileLocationBase.cpp
//
// Basic file location resolving: Uses simple path concatenation logic.
// Exposes basic path functions for convenience.
// For more details on resolving file location, refer to section "File Location" inside
// "Going Further > Overriding Managers > Streaming / Stream Manager > Low-Level I/O"
// of the SDK documentation. 
//
//////////////////////////////////////////////////////////////////////

#include "AK/SoundEngine/SampleLowLevelIO/Win32/stdafx.h"
#include "AkFileLocationBase.h"

#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#ifdef AK_SUPPORT_WCHAR
#include <wchar.h>
#endif //AK_SUPPORT_WCHAR
#include <stdio.h>
#include <AK/Tools/Common/AkAssert.h>

#include "AK/SoundEngine/SampleLowLevelIO/Win32/AkFileHelpers.h"

#include "AkGeneratedSoundBanksResolver.h"

#define MAX_NUMBER_STRING_SIZE      (10)    // 4G
#define ID_TO_STRING_FORMAT_BANK    AKTEXT("%u.bnk")
#define ID_TO_STRING_FORMAT_WEM     AKTEXT("%u.wem")
#define MAX_EXTENSION_SIZE          (4)     // .xxx
#define MAX_FILETITLE_SIZE          (MAX_NUMBER_STRING_SIZE+MAX_EXTENSION_SIZE+1)   // null-terminated

template< class TYPE > inline
const TYPE& AkTemplMax( const TYPE& in_left, const TYPE& in_right )
{
	return ( in_left < in_right ) ? in_right : in_left;
}


CAkFileLocationBase::CAkFileLocationBase()
{
	m_szBasePath[0] = '\0';
    m_szBankPath[0] = '\0';
    m_szAudioSrcPath[0] = '\0';
}

CAkFileLocationBase::~CAkFileLocationBase()
{
}

// String overload.
// Returns AK_Success if input flags are supported and the resulting path is not too long.
// Returns AK_Fail otherwise.
AKRESULT CAkFileLocationBase::GetFullFilePath(
	const AkOSChar*		in_pszFileName,		// File name.
	AkFileSystemFlags * in_pFlags,			// Special flags. Can be NULL.
	AkOpenMode			in_eOpenMode,		// File open mode (read, write, ...).
	AkOSChar*			out_pszFullFilePath // Full file path.
	)
{
    if ( !in_pszFileName )
    {
        AKASSERT( !"Invalid file name" );
        return AK_InvalidParameter;
    }

	size_t fileNameLen = AKPLATFORM::OsStrLen(in_pszFileName);
	if (AKPLATFORM::IsAbsolutePath(in_pszFileName, fileNameLen))
	{
		// No need to concat anything, the path is already resolved
		AKPLATFORM::SafeStrCpy(out_pszFullFilePath, in_pszFileName, AK_MAX_PATH);
		return AK_Success;
	}

	// Prepend string path (basic file system logic).
	AK::StringBuilder fullFilePath(out_pszFullFilePath, AK_MAX_PATH);
	fullFilePath.Append(m_szBasePath, AKPLATFORM::OsStrLen(m_szBasePath));

    if ( in_pFlags 
		&& in_eOpenMode == AK_OpenModeRead )
    {
		AK::ResolveGeneratedSoundBanksPath(fullFilePath, in_pszFileName, in_pFlags, false);
		
		// Note: Standard streaming files do not use this overload. On the other hand, streaming external 
		// sources use it if you use AkExternalSourceInfo::szFile instead of AkExternalSourceInfo::idFile.		

		// Externally supplied source (see External Sources in SDK doc)
		// In this sample, we will assume that the external source file name in_pszFileName
		// must be used as is (e.g. "myExternalSourceFile.wem").  If you use the External Source feature
		// you should modify this section to handle your FileIDs properly.
		/*if (in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC_EXTERNAL)
		{
			
		}*/
	}
	else
	{
		// Just append the file name.
		fullFilePath.Append(in_pszFileName, AKPLATFORM::OsStrLen(in_pszFileName));
	}

	if (fullFilePath.Length() >= fullFilePath.BufferSize())
	{
		AKASSERT(!"File path string exceeds maximum length!");
		return AK_Fail;
	}
	return AK_Success;
}

// ID overload. 
// The name of the file will be formatted as ID.ext. This is meant to be used with options
// "Use SoundBank Names" unchecked, and/or "Copy Streamed Files" in the SoundBank Settings.
// For more details, refer to the SoundBank Settings in Wwise Help, and to section "Identifying Banks" inside
// "Sound Engine Integration Walkthrough > Integrate Wwise Elements into Your Game > Integrating Banks > 
// Integration Details - Banks > General Information" of the SDK documentation.
// Returns AK_Success if input flags are supported and the resulting path is not too long.
// Returns AK_Fail otherwise.
AKRESULT CAkFileLocationBase::GetFullFilePath(
	AkFileID			in_fileID,			// File ID.
	AkFileSystemFlags *	in_pFlags,			// Special flags. 
	AkOpenMode			in_eOpenMode,		// File open mode (read, write, ...).
	AkOSChar *			out_pszFullFilePath	// Full file path.
	)
{
	AkOSChar pszTitle[MAX_FILETITLE_SIZE + 1];
	AK::ConvertFileIdToFilename(pszTitle, MAX_FILETITLE_SIZE, in_pFlags->uCodecID, in_fileID);
	return GetFullFilePath(pszTitle, in_pFlags, in_eOpenMode, out_pszFullFilePath);
}

AKRESULT CAkFileLocationBase::SetBasePath(
    const AkOSChar*   in_pszBasePath
    )
{
	size_t len = AKPLATFORM::OsStrLen(in_pszBasePath) + 2;	//+2 for possible missing slash
	if (len + AkTemplMax(AKPLATFORM::OsStrLen(m_szBankPath), AKPLATFORM::OsStrLen(m_szAudioSrcPath)) + AKPLATFORM::OsStrLen(AK::StreamMgr::GetCurrentLanguage()) + 1 >= AK_MAX_PATH)
	{
		return AK_InvalidParameter;
	}

	//Copy the base path EVEN if the directory does not exist.
	AKPLATFORM::SafeStrCpy( m_szBasePath, in_pszBasePath, AK_MAX_PATH );
	if (len > 2) // Ensure path has terminating separator, and validate existence of directory, except for an empty directory (==current directory)
	{
		if (m_szBasePath[len - 3] != (AkOSChar)AK_PATH_SEPARATOR[0])
		{
			m_szBasePath[len - 2] = (AkOSChar)AK_PATH_SEPARATOR[0];
			m_szBasePath[len - 1] = 0;
		}

		AKRESULT eDirectoryResult = CheckDirectoryExists(in_pszBasePath);
		if (eDirectoryResult != AK_NotImplemented) // AK_NotImplemented could be returned and should be ignored.
			return eDirectoryResult;
	}
	
	return AK_Success;
}

AKRESULT CAkFileLocationBase::SetBankPath(
    const AkOSChar*   in_pszBankPath
    )
{
    if ( AKPLATFORM::OsStrLen( m_szBasePath ) + AkTemplMax( AKPLATFORM::OsStrLen( in_pszBankPath ), AKPLATFORM::OsStrLen( m_szAudioSrcPath ) ) + AKPLATFORM::OsStrLen( AK::StreamMgr::GetCurrentLanguage() ) + 1 >= AK_MAX_PATH )
	{
		return AK_InvalidParameter;
	}
	AKPLATFORM::SafeStrCpy( m_szBankPath, in_pszBankPath, AK_MAX_PATH );
	return AK_Success;
}

AKRESULT CAkFileLocationBase::SetAudioSrcPath(
    const AkOSChar*   in_pszAudioSrcPath
    )
{
    if ( AKPLATFORM::OsStrLen( m_szBasePath ) + AkTemplMax( AKPLATFORM::OsStrLen( m_szBankPath ), AKPLATFORM::OsStrLen( in_pszAudioSrcPath ) ) + AKPLATFORM::OsStrLen( AK::StreamMgr::GetCurrentLanguage() ) + 1 >= AK_MAX_PATH )
	{
		return AK_InvalidParameter;
	}
	AKPLATFORM::SafeStrCpy( m_szAudioSrcPath, in_pszAudioSrcPath, AK_MAX_PATH );
	return AK_Success;
}

