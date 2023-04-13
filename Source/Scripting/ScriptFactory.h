#pragma once
#ifndef SCRIPTFACTORY_INCLUDED
#define SCRIPTFACTORY_INCLUDED
#include "ObjectFactorySystem/ObjectFactorySystem.h"

struct IScript;
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
	bool MainLoop();
	IScript* CreateScript(GameObject* owner, const char* path);
	ObjectId GetScript(const char* name);
	virtual void OnConstructorsAdded();
private:
	void IncludeDirs();


	//Runtime Systems
	ICompilerLogger* m_pCompilerLogger;
	IRuntimeObjectSystem* m_pRuntimeObjectSystem;
	IScript* m_pScript;

	//Runtime object
	SystemTable* g_SystemTable;
};
#endif // SCRIPTFACTORYY_INCLUDED