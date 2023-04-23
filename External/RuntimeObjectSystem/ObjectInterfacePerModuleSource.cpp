//
// Copyright (c) 2010-2011 Matthew Jack and Doug Binks
//
// This software is provided 'as-is', without any express or implied
// warranty.  In no event will the authors be held liable for any damages
// arising from the use of this software.
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.

// ObjectInterfaceOerDllSource.cpp : Defines the entry point for the DLL application.
#include "ObjectInterfacePerModule.h"
#include "FileSystemUtils.h"

PerModuleInterface* PerModuleInterface::ms_pObjectManager = NULL;

SystemTable* PerModuleInterface::g_pSystemTable = 0;
IRuntimeObjectSystem* PerModuleInterface::g_pRuntimeObjectSystem = 0;

extern "C" 
#ifdef _WIN32
	__declspec(dllexport)	//should create file with export import macros etc.
#else
__attribute__((visibility("default")))
#endif
IPerModuleInterface* GetPerModuleInterface()
{
	return PerModuleInterface::GetInstance();
}

PerModuleInterface* PerModuleInterface::GetInstance()
{
	if( !ms_pObjectManager )
	{
		ms_pObjectManager = new PerModuleInterface;
	}
	return ms_pObjectManager;
}

void PerModuleInterface::AddConstructor( IObjectConstructor* pConstructor )
{
	m_ObjectConstructors.push_back( pConstructor );
}

std::vector<IObjectConstructor*>& PerModuleInterface::GetConstructors()
{
	return m_ObjectConstructors;
}

void PerModuleInterface::SetProjectIdForAllConstructors( unsigned short projectId_ )
{
    for( size_t i = 0; i < m_ObjectConstructors.size(); ++i )
    {
        m_ObjectConstructors[ i ]->SetProjectId( projectId_ );
    }
}


void PerModuleInterface::SetSystemTable( SystemTable* pSystemTable )
{
	g_pSystemTable = pSystemTable;
}

void PerModuleInterface::SetRuntimeObjectSystem( IRuntimeObjectSystem* pRuntimeObjectSystem_ )
{
	g_pRuntimeObjectSystem = pRuntimeObjectSystem_;
}

PerModuleInterface::PerModuleInterface()
{
	//ensure this file gets compiled
	AddRequiredSourceFiles( __FILE__ );
}

const std::vector<const char*>& PerModuleInterface::GetRequiredSourceFiles() const
{
	return m_RequiredSourceFiles;
}

void PerModuleInterface::AddRequiredSourceFiles( const char* file_ )
{
	m_RequiredSourceFiles.push_back( file_ );
}

std::string PerModuleInterface::GetOSCanonicalCleanPath( const char* compilePath_, const char* filename_ )
{
	FileSystemUtils::Path path;
	path = compilePath_;
	path = path / filename_;
	path = path.DelimitersToOSDefault();
	path = path.GetCleanPath();
	path.ToOSCanonicalCase();
	return path.m_string;
}

std::string PerModuleInterface::GetOSCanonicalCleanPath( const char* filename_ )
{
	FileSystemUtils::Path path;
	path = filename_;
	path = path.DelimitersToOSDefault();
	path = path.GetCleanPath();
	path.ToOSCanonicalCase();
	return path.m_string;
}