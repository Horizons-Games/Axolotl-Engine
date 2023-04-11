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

#include "AkGeneratedSoundBanksResolver.h"

#define ID_TO_STRING_FORMAT_BANK    AKTEXT("%u.bnk")
#define ID_TO_STRING_FORMAT_WEM     AKTEXT("%u.wem")

enum AkFileKind
{
	AkFileKind_UserBank = 0,
	AkFileKind_EventBank,
	AkFileKind_BusBank,
	AkFileKind_MediaFile,
	AkFileKind_Other,

	AkFileKind_NUM
};

static const AkOSChar* kFileKindFolders[AkFileKind_NUM] = {
	AKTEXT(""),
	AKTEXT("Event"),
	AKTEXT("Bus"),
	AKTEXT("Media"),
	AKTEXT("")
};

static const AkFileKind FileKindFromCodecID(AkUInt32 in_codecID)
{

	switch (in_codecID)
	{
	case AKCODECID_BANK:
		return AkFileKind_UserBank;

	case AKCODECID_FILE_PACKAGE:
	case AKCODECID_PROFILERCAPTURE:
	case AKCODECID_ANALYSISFILE:
	case AKCODECID_MEMORYMGR_DUMP:
		return AkFileKind_Other;

	case AKCODECID_BANK_EVENT:
		return AkFileKind_EventBank;
	case AKCODECID_BANK_BUS:
		return AkFileKind_BusBank;
	default:
		return AkFileKind_MediaFile;
	}
}

void AK::ConvertFileIdToFilename(AkOSChar* out_pszTitle, AkUInt32 in_pszTitleMaxLen, AkUInt32 in_codecId, AkFileID in_fileID)
{
	if (AK::IsBankCodecID(in_codecId))
		AK_OSPRINTF(out_pszTitle, in_pszTitleMaxLen, ID_TO_STRING_FORMAT_BANK, (unsigned int)in_fileID);
	else
		AK_OSPRINTF(out_pszTitle, in_pszTitleMaxLen, ID_TO_STRING_FORMAT_WEM, (unsigned int)in_fileID);
}

void AK::ResolveGeneratedSoundBanksPath(StringBuilder &dest, const AkOSChar* in_pszFileName, AkFileSystemFlags * in_pFlags, bool in_bUseSubfoldering)
{
	AkFileKind kind = FileKindFromCodecID(in_pFlags->uCodecID);
	size_t uSeparatorLen = AKPLATFORM::OsStrLen(AK_PATH_SEPARATOR);

	// Add bank type directory name, only for "native" (read: non-external) files
	if (in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC)
	{
		const AkOSChar* pBankFolder = kFileKindFolders[kind];
		size_t uFolderStrLen = AKPLATFORM::OsStrLen(pBankFolder);
		if (uFolderStrLen > 0)
		{
			dest.Append(pBankFolder, uFolderStrLen);
			dest.Append(AK_PATH_SEPARATOR, uSeparatorLen);
		}
	}

	// Add language directory name if needed (this applies to both native and external files)
	if (in_pFlags->bIsLanguageSpecific)
	{
		size_t uLanguageStrLen = AKPLATFORM::OsStrLen(AK::StreamMgr::GetCurrentLanguage());
		if (uLanguageStrLen > 0)
		{
			dest.Append(AK::StreamMgr::GetCurrentLanguage(), uLanguageStrLen);
			dest.Append(AK_PATH_SEPARATOR, uSeparatorLen);
		}
	}

	// Files in kind sub-folder can be further sub-divided (non-external files only)
	if (in_pFlags->uCompanyID == AKCOMPANYID_AUDIOKINETIC && in_bUseSubfoldering)
	{
		switch (kind)
		{
		case AkFileKind_MediaFile:
		case AkFileKind_EventBank:
		case AkFileKind_BusBank:
		{
			AKASSERT(in_pFlags->uDirectoryHash != AK_INVALID_FILE_ID);

			// 12 seems to be the minimum required size to convert all unsigned int.
			AkOSChar szSubfolder[12] = {};
			int formatResult = AK_OSPRINTF(szSubfolder, 12, AKTEXT("%u"), (unsigned int)in_pFlags->uDirectoryHash);
			AKASSERT(formatResult >= 1 && "Could not convert directory hash.");

			// Append the two first digits of the directory hash to the path.
			dest.Append(szSubfolder, AkMin(formatResult, 2));
			dest.Append(AK_PATH_SEPARATOR, uSeparatorLen);
		}

			break;
		default:
			break;
		}
	}
	dest.Append(in_pszFileName, AKPLATFORM::OsStrLen(in_pszFileName));
}
