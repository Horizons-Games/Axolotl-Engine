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

#include <AK/SoundEngine/Common/AkSoundEngine.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AK/SoundEngine/Common/AkStreamMgrModule.h>

namespace AK
{
	class StringBuilder
	{
	public:
		StringBuilder(AkOSChar* in_pBuffer, size_t in_uBufferSize)
			: m_pBuffer(in_pBuffer)
			, m_uBufferSize(in_uBufferSize)
			, m_uDataLength(0)
		{
			AKASSERT(in_uBufferSize > 0);
			in_pBuffer[0] = '\0';
		}

		/// Appends characters in the range [str, str + length). This range can contain null characters or not.
		/// @returns AK_Success on success or AK_MaxReached if there is not enough space.
		inline AKRESULT Append(const AkOSChar* str, size_t length)
		{
			const size_t newLength = m_uDataLength + length;
			if (newLength >= m_uBufferSize)
				return AK_MaxReached;

			AKPLATFORM::AkMemCpy(m_pBuffer + m_uDataLength, str, (AkUInt32)(length * sizeof(AkOSChar)));
			m_pBuffer[newLength] = 0;
			m_uDataLength += AKPLATFORM::OsStrLen(m_pBuffer + m_uDataLength);
			return AK_Success;
		}

		/// Appends a null terminating string
		/// @returns AK_Success on success or AK_MaxReached if there is not enough space.
		inline AKRESULT Append(const AkOSChar* str)
		{
			const size_t length = AKPLATFORM::OsStrLen(str);
			const size_t newLength = m_uDataLength + length;
			if (newLength >= m_uBufferSize)
				return AK_MaxReached;

			AKPLATFORM::AkMemCpy(m_pBuffer + m_uDataLength, str, (AkUInt32)(length * sizeof(AkOSChar)));
			m_uDataLength = newLength;
			m_pBuffer[newLength] = 0;
			return AK_Success;
		}

		inline AkOSChar* Buffer() { return m_pBuffer; }
		inline const AkOSChar* Buffer() const { return m_pBuffer; }
		inline size_t BufferSize() const { return m_uBufferSize; }
		inline size_t Length() const { return m_uDataLength; }

		inline AkOSChar& operator[](size_t index) { return m_pBuffer[index]; }
		inline AkOSChar operator[](size_t index) const { return m_pBuffer[index]; }

		void Truncate(size_t uLength)
		{
			AKASSERT(uLength <= m_uDataLength);
			m_uDataLength = uLength;
			m_pBuffer[uLength] = '\0';
		}

	private:
		AkOSChar* m_pBuffer;
		size_t m_uBufferSize;
		size_t m_uDataLength;
	};

	void ConvertFileIdToFilename(AkOSChar* out_pszTitle, AkUInt32 in_pszTitleMaxLen, AkUInt32 in_codecId, AkFileID in_fileID);
	void ResolveGeneratedSoundBanksPath(StringBuilder &dest, const AkOSChar* in_pszFileName, AkFileSystemFlags * in_pFlags, bool bUseSubfoldering);
}
