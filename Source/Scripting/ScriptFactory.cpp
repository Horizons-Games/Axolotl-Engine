#include "ScriptFactory.h"
#include "RuntimeObjectSystem.h"
#include "LogSystem.h"
#include "IObject.h"
#include "IScript.h"
#include "Script.h"
#include "EngineLog.h"
#include "FileSystemUtils.h"
#include "GameObject/GameObject.h"
#include "Components/ComponentScript.h"
#include "SystemTable.h"
#include <conio.h>


#if defined _WINDOWS_ && defined GetObject
#undef GetObject
#endif

ScriptFactory::ScriptFactory() : m_pCompilerLogger(nullptr), m_pRuntimeObjectSystem(nullptr)
{
	
}


ScriptFactory::~ScriptFactory()
{
	if (m_pRuntimeObjectSystem)
	{
		//Aixo falla
		//m_pRuntimeObjectSystem->CleanObjectFiles();
	}
	if (m_pRuntimeObjectSystem && m_pRuntimeObjectSystem->GetObjectFactorySystem())
	{
		m_pRuntimeObjectSystem->GetObjectFactorySystem()->RemoveListener(this);

	}
	delete m_pRuntimeObjectSystem;
	delete m_pCompilerLogger;
}


bool ScriptFactory::Init()
{
	//Initialise the RuntimeObjectSystem
	m_pRuntimeObjectSystem = new RuntimeObjectSystem;
	g_SystemTable = new SystemTable();

	m_pCompilerLogger = new LogSystem();

	m_pRuntimeObjectSystem->SetAdditionalCompileOptions("/std:c++20");
	//Tener un RuntimeObjectSystem por cada script
	if (!m_pRuntimeObjectSystem->Initialise(m_pCompilerLogger, 0))
	{
		m_pRuntimeObjectSystem = 0;
		return false;
	}

	m_pRuntimeObjectSystem->GetObjectFactorySystem()->AddListener(this);

	IncludeDirs();

	return true;
}

void ScriptFactory::AddScript(const char* path)
{
	m_pRuntimeObjectSystem->AddToRuntimeFileList(path, 0);
	m_pRuntimeObjectSystem->CleanObjectFiles();
	RecompileAll();
}

void ScriptFactory::RecompileAll()
{
	m_pRuntimeObjectSystem->CompileAll(true);
}


std::vector<const char*> ScriptFactory::GetConstructors()
{
	AUDynArray<IObjectConstructor*> constructors;
	m_pRuntimeObjectSystem->GetObjectFactorySystem()->GetAll(constructors);
	std::vector<const char*> constructorsName;
	for (int i = 0; i < constructors.Size(); ++i)
	{
		constructorsName.push_back(constructors[i]->GetName());
	}
	return constructorsName;
}

bool ScriptFactory::IsCompiling()
{
	return m_pRuntimeObjectSystem->GetIsCompiling();
}

bool ScriptFactory::IsCompiled()
{
	return m_pRuntimeObjectSystem->GetIsCompiledComplete();
}

void ScriptFactory::LoadCompiledModules() 
{
	m_pRuntimeObjectSystem->LoadCompiledModule();
}

void ScriptFactory::UpdateNotifier()
{
	const float deltaTime = 1.0f;
	m_pRuntimeObjectSystem->GetFileChangeNotifier()->Update(deltaTime);
}



void ScriptFactory::OnConstructorsAdded()
{
	/*AUDynArray<IObjectConstructor*> constructors = AUDynArray<IObjectConstructor*>();
	m_pRuntimeObjectSystem->GetObjectFactorySystem()->GetAll(constructors);
	for (size_t i = 0; i < constructors.Size(); i++)
	{
		IObject* object = constructors[i]->GetConstructedObject(InterfaceIDEnum::IID_IOBJECT);
		IScript* script;
		//This have to be the script
		object->GetInterface(&script);
		if (0 == script)
		{
			delete object;
			m_pCompilerLogger->LogError("Error - no updateable interface found\n");
		}
	}*/
}

IScript* ScriptFactory::GetScript(const char* name)
{
	IObjectConstructor* pCtor = m_pRuntimeObjectSystem->GetObjectFactorySystem()->GetConstructor(name);
	ObjectId objectId;
	if (pCtor)
	{
		IObject* pObj = pCtor->Construct();
		IScript* script;
		pObj->GetInterface(&script);
		if (script == nullptr)
		{
			delete pObj;
			m_pCompilerLogger->LogError("Error - no updateable interface found\n");
			//return false;
		}
		//objectId = pObj->GetObjectId();
		//return objectId;
		return script;
	}
	return nullptr;
}

void ScriptFactory::IncludeDirs() {
	FileSystemUtils::Path basePath = m_pRuntimeObjectSystem->FindFile(__FILE__).ParentPath().ParentPath().ParentPath() / "Source";
	FileSystemUtils::Path source = basePath;
	m_pRuntimeObjectSystem->AddIncludeDir(source.c_str());

	FileSystemUtils::Path dataModels = basePath / "DataModels";
	m_pRuntimeObjectSystem->AddIncludeDir(dataModels.c_str());

	FileSystemUtils::Path modules = basePath / "Modules";
	m_pRuntimeObjectSystem->AddIncludeDir(modules.c_str());

	FileSystemUtils::Path scripting = basePath / "Scripting";
	m_pRuntimeObjectSystem->AddIncludeDir(scripting.c_str());

	FileSystemUtils::Path external = basePath.ParentPath() / "External";
	m_pRuntimeObjectSystem->AddIncludeDir(external.c_str());

	FileSystemUtils::Path runtimeCompiler = basePath.ParentPath() / "External" / "RuntimeCompiler" / "Include";
	m_pRuntimeObjectSystem->AddIncludeDir(runtimeCompiler.c_str());

	FileSystemUtils::Path runtimeObjectSystem = basePath.ParentPath() / "External" / "RuntimeObjectSystem";
	m_pRuntimeObjectSystem->AddIncludeDir(runtimeObjectSystem.c_str());

	FileSystemUtils::Path mathGeoLib = external / "MathGeoLib" / "Include";
	m_pRuntimeObjectSystem->AddIncludeDir(mathGeoLib.c_str());


	FileSystemUtils::Path rapidjson = external / "rapidjson" / "include";
	m_pRuntimeObjectSystem->AddIncludeDir(rapidjson.c_str());


	FileSystemUtils::Path externalLibs = source / "x64" / "DebugEngine";
	m_pRuntimeObjectSystem->AddLibraryDir(externalLibs.c_str());

	FileSystemUtils::Path game = source.ParentPath() / "Game";
	m_pRuntimeObjectSystem->AddLibraryDir(game.c_str());


	FileSystemUtils::Path debugDebug = source / "build" / "x64" / "Debug";
	m_pRuntimeObjectSystem->AddLibraryDir(debugDebug.c_str());

	FileSystemUtils::Path optick = external / "Optick" / "lib" / "x64" / "debug";
	m_pRuntimeObjectSystem->AddLibraryDir(optick.c_str());

	FileSystemUtils::Path directx = external / "DirectXTex" / "libs" / "Debug_lib";
	m_pRuntimeObjectSystem->AddLibraryDir(directx.c_str());
}