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

#include <AK/SoundEngine/Common/AkStreamMgrModule.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#ifdef AK_SUPPORT_WCHAR
#include <wchar.h>
#endif //AK_SUPPORT_WCHAR
#include <stdio.h>
#include <AK/Tools/Common/AkAssert.h>
#include <AK/Tools/Common/AkObject.h>

#include "AK/SoundEngine/SampleLowLevelIO/Win32/AkFileHelpers.h"
#include "AkMultipleFileLocation.h"
#include "AkGeneratedSoundBanksResolver.h"

#define MAX_NUMBER_STRING_SIZE      (10)    // 4G
#define MAX_EXTENSION_SIZE          (4)     // .xxx
#define MAX_FILETITLE_SIZE          (MAX_NUMBER_STRING_SIZE+MAX_EXTENSION_SIZE+1)   // null-terminated

CAkMultipleFileLocation::CAkMultipleFileLocation()
	: m_bUseSubfoldering(false)
{
}

void CAkMultipleFileLocation::Term()
{
	if (!m_Locations.IsEmpty())
	{
		FilePath *p = (*m_Locations.Begin());
		while (p)
		{
			FilePath *next = p->pNextLightItem;
			AkDelete(AkMemID_Streaming, p);
			p = next;
		}
	}
	m_Locations.Term();
}

AKRESULT CAkMultipleFileLocation::Open(
	const AkOSChar* in_pszFileName,     // File name.
	AkOpenMode      in_eOpenMode,       // Open mode.
	AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
	bool			in_bOverlapped,		// Overlapped IO open
	AkFileDesc &    out_fileDesc        // Returned file descriptor.
	)
{
	AkOSChar szFullFilePath[AK_MAX_PATH];
	AKRESULT res = AK_FileNotFound;
	if (m_Locations.IsEmpty())
	{
		// Get the full file path, using path concatenation logic.

		if (GetFullFilePath(in_pszFileName, in_pFlags, in_eOpenMode, szFullFilePath ) == AK_Success)
		{
			res = CAkFileHelpers::Open(szFullFilePath, in_eOpenMode, in_bOverlapped, out_fileDesc);
			if (res == AK_Success)
			{
				// iFileSize must be set by the OpenPolicy.
				AKASSERT(out_fileDesc.iFileSize != 0 && (in_eOpenMode == AK_OpenModeRead || in_eOpenMode == AK_OpenModeReadWrite) || !(in_eOpenMode == AK_OpenModeRead || in_eOpenMode == AK_OpenModeReadWrite));
				return AK_Success;
			}
		}
	}
	else
	{
		for (AkListBareLight<FilePath>::Iterator it = m_Locations.Begin(); it != m_Locations.End(); ++it)
		{
			// Get the full file path, using path concatenation logic.

			if (GetFullFilePath(in_pszFileName, in_pFlags, in_eOpenMode, szFullFilePath, (*it)->szPath) == AK_Success)
			{
				res = CAkFileHelpers::Open(szFullFilePath, in_eOpenMode, in_bOverlapped, out_fileDesc);
				if (res == AK_Success)
				{
					// iFileSize must be set by the OpenPolicy.
					AKASSERT(out_fileDesc.iFileSize != 0 && (in_eOpenMode == AK_OpenModeRead || in_eOpenMode == AK_OpenModeReadWrite) || !(in_eOpenMode == AK_OpenModeRead || in_eOpenMode == AK_OpenModeReadWrite));
					return AK_Success;
				}
			}
		}
	}
	return res;    
}

AKRESULT CAkMultipleFileLocation::Open( 
										AkFileID        in_fileID,          // File ID.
										AkOpenMode      in_eOpenMode,       // Open mode.
										AkFileSystemFlags * in_pFlags,      // Special flags. Can pass NULL.
										bool			in_bOverlapped,		// Overlapped IO open
										AkFileDesc &    out_fileDesc        // Returned file descriptor.
										)
{
	if ( !in_pFlags ||
		!(in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC || in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC_EXTERNAL))
	{
		AKASSERT( !"Unhandled file type" );
		return AK_InvalidFile;
	}

	AkOSChar pszTitle[MAX_FILETITLE_SIZE+1];
	AK::ConvertFileIdToFilename(pszTitle, MAX_FILETITLE_SIZE, in_pFlags->uCodecID, in_fileID);

	return Open(pszTitle, in_eOpenMode, in_pFlags, in_bOverlapped, out_fileDesc);
}

// String overload.
// Returns AK_Success if input flags are supported and the resulting path is not too long.
// Returns AK_Fail otherwise.
AKRESULT CAkMultipleFileLocation::GetFullFilePath(
	const AkOSChar*		in_pszFileName,		// File name.
	AkFileSystemFlags * in_pFlags,			// Special flags. Can be NULL.
	AkOpenMode			in_eOpenMode,		// File open mode (read, write, ...).
	AkOSChar*			out_pszFullFilePath, // Full file path.
	AkOSChar*			in_pszBasePath		// Base path to use, might be null	
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
		CAkFileHelpers::NormalizeDirectorySeparators(out_pszFullFilePath);
		return AK_Success;
	}

	// Prepend string path (basic file system logic).
	AK::StringBuilder fullFilePath(out_pszFullFilePath, AK_MAX_PATH);

    // Compute file name with file system paths.

	if (in_pszBasePath != NULL)
	{
		fullFilePath.Append(in_pszBasePath, AKPLATFORM::OsStrLen(in_pszBasePath));
	}
	else
	{
		if (m_Locations.IsEmpty())
		{
			fullFilePath.Truncate(0);
		}
		else
		{
			AkListBareLight<FilePath>::Iterator it = m_Locations.Begin();
			fullFilePath.Append((*it)->szPath, AKPLATFORM::OsStrLen((*it)->szPath));
		}
	}


    if ( in_pFlags && in_eOpenMode == AK_OpenModeRead )
    {
		AK::ResolveGeneratedSoundBanksPath(fullFilePath, in_pszFileName, in_pFlags, m_bUseSubfoldering);
	}
	else
	{
		fullFilePath.Append(in_pszFileName, fileNameLen);
	}

	if (fullFilePath.Length() >= fullFilePath.BufferSize())
	{
		AKASSERT(!"File path string exceeds maximum length!");
		return AK_Fail;
	}

	CAkFileHelpers::NormalizeDirectorySeparators(out_pszFullFilePath);

	return AK_Success;
}

AKRESULT CAkMultipleFileLocation::OutputSearchedPaths(
	const AkOSChar* in_pszFileName,		
	AkFileSystemFlags* in_pFlags,
	AkOpenMode in_eOpenMode,
	AkOSChar* out_searchedPath,
	AkInt32 in_pathSize
)
{
	if (!out_searchedPath)
		return AK_Fail;
	//Make sure the out_searchedPath is empty
	memset(out_searchedPath, 0, in_pathSize);

	AkOSChar szFullFilePath[AK_MAX_PATH];
	if (m_Locations.IsEmpty())
	{
		// Get the full file path, using path concatenation logic.

		if (GetFullFilePath(in_pszFileName, in_pFlags, in_eOpenMode, szFullFilePath) == AK_Success)
		{
			AKPLATFORM::SafeStrCat(out_searchedPath, szFullFilePath, in_pathSize);
		}
	}
	else
	{
		for (AkListBareLight<FilePath>::Iterator it = m_Locations.Begin(); it != m_Locations.End(); ++it)
		{
			// Get the full file path, using path concatenation logic.

			if (GetFullFilePath(in_pszFileName, in_pFlags, in_eOpenMode, szFullFilePath, (*it)->szPath) == AK_Success)
			{
				AKPLATFORM::SafeStrCat(out_searchedPath, szFullFilePath, in_pathSize);
				AKPLATFORM::SafeStrCat(out_searchedPath, AKTEXT("; "), in_pathSize);
			}
		}
	}
	return AKPLATFORM::OsStrLen(out_searchedPath) > 0 ? AK_Success : AK_Fail;
}

AKRESULT CAkMultipleFileLocation::OutputSearchedPaths(
	AkFileID in_fileID,
	AkFileSystemFlags* in_pFlags,
	AkOpenMode in_eOpenMode,
	AkOSChar* out_searchedPath,
	AkInt32 in_pathSize
)
{
	if (!out_searchedPath)
		return AK_Fail;
	if (!in_pFlags ||
		!(in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC || in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC_EXTERNAL))
	{
		AKASSERT(!"Unhandled file type");
		return AK_Fail;
	}
	AkOSChar pszTitle[MAX_FILETITLE_SIZE + 1];
	AK::ConvertFileIdToFilename(pszTitle, MAX_FILETITLE_SIZE, in_pFlags->uCodecID, in_fileID);

	AkOSChar szFullFilePath[AK_MAX_PATH];
	if (m_Locations.IsEmpty())
	{
		// Get the full file path, using path concatenation logic.
		if (GetFullFilePath(pszTitle, in_pFlags, in_eOpenMode, szFullFilePath) == AK_Success)
		{
			AKPLATFORM::SafeStrCat(out_searchedPath, szFullFilePath, in_pathSize);
		}
	}
	else
	{
		for (AkListBareLight<FilePath>::Iterator it = m_Locations.Begin(); it != m_Locations.End(); ++it)
		{
			// Get the full file path, using path concatenation logic.
			if (GetFullFilePath(pszTitle, in_pFlags, in_eOpenMode, szFullFilePath, (*it)->szPath) == AK_Success)
			{
				AKPLATFORM::SafeStrCat(out_searchedPath, szFullFilePath, in_pathSize);
				AKPLATFORM::SafeStrCat(out_searchedPath, AKTEXT("; "), in_pathSize);
			}
		}
	}
	return AKPLATFORM::OsStrLen(out_searchedPath) > 0 ? AK_Success : AK_Fail;
}

AKRESULT CAkMultipleFileLocation::AddBasePath(const AkOSChar* in_pszBasePath)
{
	AkUInt32 origLen = (AkUInt32)AKPLATFORM::OsStrLen(in_pszBasePath);
    AkUInt32 newByteSize = origLen + 1;	// One for the trailing \0
    if(origLen && in_pszBasePath[origLen - 1] != AK_PATH_SEPARATOR[0])
    {
        newByteSize++; // Add space for a trailing slash
    }

    // Make sure the new base path is not too long in case language gets appended
    // Format of the test is: basePath/Language/.
	if ( origLen + 1 + AKPLATFORM::OsStrLen( AK::StreamMgr::GetCurrentLanguage() + 1 ) >= AK_MAX_PATH )
		return AK_InvalidParameter;

	FilePath * pPath = (FilePath*)AkAlloc(AkMemID_Streaming, sizeof(FilePath) + sizeof(AkOSChar)*(newByteSize - 1));
	if (pPath == NULL)
		return AK_InsufficientMemory;

	// Copy the base path EVEN if the directory does not exist.
	AKPLATFORM::SafeStrCpy( pPath->szPath, in_pszBasePath, origLen + 1);
    
    // Add the trailing slash, if necessary
	if (origLen)// UWP is not soo happy of the trailing slash on an empty string.
	{
		if (in_pszBasePath[origLen - 1] != AK_PATH_SEPARATOR[0])
		{
			pPath->szPath[origLen] = AK_PATH_SEPARATOR[0];
			pPath->szPath[origLen + 1] = 0;
		}
	}
	pPath->pNextLightItem = NULL;
	m_Locations.AddFirst(pPath);

	AKRESULT eDirectoryResult = CAkFileHelpers::CheckDirectoryExists(in_pszBasePath);
	if (eDirectoryResult != AK_NotImplemented) // AK_NotImplemented could be returned and should be ignored.
		return eDirectoryResult;

	return AK_Success;
}
