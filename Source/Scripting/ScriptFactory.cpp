#include "ScriptFactory.h"
#include "AxoLog.h"
#include "Components/ComponentScript.h"
#include "AxoLog.h"
#include "FileSystemUtils.h"
#include "GameObject/GameObject.h"
#include "IObject.h"
#include "IScript.h"
#include "LogSystem.h"
#include "RuntimeObjectSystem.h"
#include "Script.h"
#include <conio.h>

#if defined _WINDOWS_ && defined GetObject
	#undef GetObject
#endif

ScriptFactory::ScriptFactory() : pCompilerLogger(nullptr), pRuntimeObjectSystem(nullptr)
{
}

ScriptFactory::~ScriptFactory()
{
	if (pRuntimeObjectSystem)
	{
		// This doesn't work. Need to check if it's actually necessary
		// pRuntimeObjectSystem->CleanObjectFiles();
	}
	if (pRuntimeObjectSystem && pRuntimeObjectSystem->GetObjectFactorySystem())
	{
		pRuntimeObjectSystem->GetObjectFactorySystem()->RemoveListener(this);
	}
	delete pRuntimeObjectSystem;
	delete pCompilerLogger;
}

bool ScriptFactory::Init()
{
	// Initialise the RuntimeObjectSystem
	pRuntimeObjectSystem = new RuntimeObjectSystem;

	pCompilerLogger = new LogSystem();

	pRuntimeObjectSystem->SetAdditionalCompileOptions("/std:c++20");
	if (!pRuntimeObjectSystem->Initialise(pCompilerLogger, nullptr))
	{
		pRuntimeObjectSystem = 0;
		return false;
	}

	pRuntimeObjectSystem->GetObjectFactorySystem()->AddListener(this);

	IncludeDirs();

	return true;
}

void ScriptFactory::AddScript(const char* path)
{
	pRuntimeObjectSystem->AddToRuntimeFileList(path, 0);
	pRuntimeObjectSystem->CleanObjectFiles();
	RecompileAll();
}

void ScriptFactory::RecompileAll()
{
	pRuntimeObjectSystem->CompileAll(true);
}

std::vector<const char*> ScriptFactory::GetConstructors()
{
	AUDynArray<IObjectConstructor*> constructors;
	pRuntimeObjectSystem->GetObjectFactorySystem()->GetAll(constructors);
	std::vector<const char*> constructorsName;
	for (int i = 0; i < constructors.Size(); ++i)
	{
		constructorsName.push_back(constructors[i]->GetName());
	}
	return constructorsName;
}

bool ScriptFactory::IsCompiling()
{
	return pRuntimeObjectSystem->GetIsCompiling();
}

bool ScriptFactory::IsCompiled()
{
	return pRuntimeObjectSystem->GetIsCompiledComplete();
}

void ScriptFactory::LoadCompiledModules()
{
	pRuntimeObjectSystem->LoadCompiledModule();
}

void ScriptFactory::UpdateNotifier()
{
	const float deltaTime = 1.0f;
	pRuntimeObjectSystem->GetFileChangeNotifier()->Update(deltaTime);
}

void ScriptFactory::OnConstructorsAdded()
{
	for (auto& idAndPointer : allScripts)
	{
		if (idAndPointer.second)
		{
			IObject* pObj = pRuntimeObjectSystem->GetObjectFactorySystem()->GetObject(idAndPointer.first);
			pObj->GetInterface(&(idAndPointer.second));
			if (idAndPointer.second == nullptr)
			{
				delete pObj;
				pCompilerLogger->LogError("Error - no updateable interface found\n");
			}
		}
	}
}

IScript* ScriptFactory::ConstructScript(const char* name)
{
	IObjectConstructor* pCtor = pRuntimeObjectSystem->GetObjectFactorySystem()->GetConstructor(name);
	ObjectId objectId;
	if (pCtor)
	{
		IObject* pObj = pCtor->Construct();
		IScript* script;
		pObj->GetInterface(&script);
		if (script == nullptr)
		{
			delete pObj;
			pCompilerLogger->LogError("Error - no updateable interface found\n");
			return nullptr;
		}
		allScripts.push_back({ pObj->GetObjectId(), script });
		return script;
	}
	return nullptr;
}

IScript* ScriptFactory::GetScript(const char* name)
{
	auto it = std::find_if(std::begin(allScripts),
						   std::end(allScripts),
						   [&name](const std::pair<ObjectId, IScript*>& pair)
						   {
							   return pair.second->GetConstructor()->GetName() == name;
						   });
	return it == std::end(allScripts) ? nullptr : it->second;
}

void ScriptFactory::IncludeDirs()
{
	FileSystemUtils::Path basePath =
		pRuntimeObjectSystem->FindFile(__FILE__).ParentPath().ParentPath().ParentPath() / "Source";
	FileSystemUtils::Path source = basePath;
	pRuntimeObjectSystem->AddIncludeDir(source.c_str());

	FileSystemUtils::Path dataModels = basePath / "DataModels";
	pRuntimeObjectSystem->AddIncludeDir(dataModels.c_str());

	FileSystemUtils::Path modules = basePath / "Modules";
	pRuntimeObjectSystem->AddIncludeDir(modules.c_str());

	FileSystemUtils::Path scripting = basePath / "Scripting";
	pRuntimeObjectSystem->AddIncludeDir(scripting.c_str());

	FileSystemUtils::Path external = basePath.ParentPath() / "External";
	pRuntimeObjectSystem->AddIncludeDir(external.c_str());

	FileSystemUtils::Path runtimeCompiler = basePath.ParentPath() / "External" / "RuntimeCompiler" / "Include";
	pRuntimeObjectSystem->AddIncludeDir(runtimeCompiler.c_str());

	FileSystemUtils::Path runtimeObjectSystem = basePath.ParentPath() / "External" / "RuntimeObjectSystem";
	pRuntimeObjectSystem->AddIncludeDir(runtimeObjectSystem.c_str());

	FileSystemUtils::Path mathGeoLib = external / "MathGeoLib" / "Include";
	pRuntimeObjectSystem->AddIncludeDir(mathGeoLib.c_str());

	FileSystemUtils::Path rapidjson = external / "rapidjson" / "include";
	pRuntimeObjectSystem->AddIncludeDir(rapidjson.c_str());

	FileSystemUtils::Path externalLibs = source / "x64" / "DebugEngine";
	pRuntimeObjectSystem->AddLibraryDir(externalLibs.c_str());

	FileSystemUtils::Path game = source.ParentPath() / "Game";
	pRuntimeObjectSystem->AddLibraryDir(game.c_str());

	FileSystemUtils::Path debugDebug = source / "build" / "x64" / "Debug";
	pRuntimeObjectSystem->AddLibraryDir(debugDebug.c_str());

	FileSystemUtils::Path optick = external / "Optick" / "lib" / "x64" / "debug";
	pRuntimeObjectSystem->AddLibraryDir(optick.c_str());

	FileSystemUtils::Path directx = external / "DirectXTex" / "libs" / "Debug_lib";
	pRuntimeObjectSystem->AddLibraryDir(directx.c_str());
}