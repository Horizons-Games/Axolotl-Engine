#pragma once
#include "ObjectInterfacePerModule.h"
#include "IScript.h"
#include "InterfacesIds.h"
#include "ISimpleSerializer.h"

#include "GameObject/GameObject.h"
#include "Application.h"

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
	virtual void Update(float deltaTime) override {};
	virtual void PostUpdate(float deltaTime) override {};
	virtual void CleanUp() override {};
};