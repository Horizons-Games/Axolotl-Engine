#pragma once

#include "ObjectFactorySystem/ObjectFactorySystem.h"
#include <vector>

class IScript;
struct IRuntimeObjectSystem;
struct SystemTable;
struct ObjectId;
class GameObject;
class ComponentScript;

class ScriptFactory : public IObjectFactoryListener
{
public:
	ScriptFactory();
	virtual ~ScriptFactory();
	bool Init();

	IScript* ConstructScript(const char* name);
	IScript* GetScript(const char* name);
	void AddScript(const char* path);
	void RecompileAll();
	void UpdateNotifier();
	void LoadCompiledModules();
	void OnConstructorsAdded() override;
	bool IsCompiling();
	bool IsCompiled();
	std::vector<const char*> GetConstructors();
private:
	void IncludeDirs();

	//Runtime Systems
	ICompilerLogger* pCompilerLogger;
	IRuntimeObjectSystem* pRuntimeObjectSystem;

	std::vector<std::pair<ObjectId, IScript*>> allScripts;
};
