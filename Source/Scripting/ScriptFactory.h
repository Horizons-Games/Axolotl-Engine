#pragma once
#ifndef SCRIPTFACTORY_INCLUDED
#define SCRIPTFACTORY_INCLUDED
#include "ObjectFactorySystem/ObjectFactorySystem.h"

struct IScript;
struct IRuntimeObjectSystem;
struct SystemTable;
struct ObjectId;
class GameObject;

class ScriptFactory : public IObjectFactoryListener
{
public:
	ScriptFactory(GameObject* o);
	virtual ~ScriptFactory();
	bool Init();
	bool MainLoop();
	void CreateScript(const char* path);
	ObjectId GetScript(const char* name);
	virtual void OnConstructorsAdded();
private:
	void IncludeDirs();

	GameObject* owner;

	//Runtime Systems
	ICompilerLogger* m_pCompilerLogger;
	IRuntimeObjectSystem* m_pRuntimeObjectSystem;

	//Runtime object
	IScript* m_pScript;
	ObjectId m_ObjectId;
	std::vector<IScript*> scripts;
	std::vector<ObjectId> objectsIds;
	SystemTable* g_SystemTable;
};
#endif // SCRIPTFACTORYY_INCLUDED