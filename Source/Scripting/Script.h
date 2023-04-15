#pragma once
#include "ObjectInterfacePerModule.h"
#include "IScript.h"
#include "InterfacesIds.h"
#include "EngineLog.h"
#include "SystemTable.h"
#include "ISimpleSerializer.h"

#include "RuntimeLinkLibrary.h"
RUNTIME_COMPILER_LINKLIBRARY("ExternalDependencies.lib");
RUNTIME_COMPILER_LINKLIBRARY("Engine.lib");
RUNTIME_COMPILER_LINKLIBRARY("OptickCore.lib");
RUNTIME_COMPILER_LINKLIBRARY("DirectXTex.lib");


class Script : public TInterface<IID_IUPDATEABLE, IScript>
{
public:
	virtual ~Script() override = default;

	virtual void Init() override {};
	virtual void Start() override {};
	virtual void PreUpdate(float deltaTime) override {};
	virtual void Update(float deltaTime) override
	{
		ENGINE_LOG("Update");
	}
	virtual void PostUpdate(float deltaTime) override {};
	virtual void CleanUp() override {};

	virtual void SetGameObject(GameObject* owner)
	{
		this->owner = owner;
	}
	virtual void SetApplication(Application* app)
	{
		this->App = app;
	}
	void Serialize(ISimpleSerializer* pSerializer) override
	{
		SERIALIZE(owner);
		SERIALIZE(App);
	}

};