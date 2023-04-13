#pragma once
#include "ObjectInterfacePerModule.h"
#include "IUpdateable.h"
#include "InterfacesIds.h"
#include "EngineLog.h"
#include "SystemTable.h"

#include "RuntimeLinkLibrary.h"
RUNTIME_COMPILER_LINKLIBRARY("ExternalDependencies.lib");
RUNTIME_COMPILER_LINKLIBRARY("Engine.lib");
RUNTIME_COMPILER_LINKLIBRARY("OptickCore.lib");
RUNTIME_COMPILER_LINKLIBRARY("DirectXTex.lib");
RUNTIME_COMPILER_LINKLIBRARY("RuntimeCompiler.lib");
RUNTIME_COMPILER_LINKLIBRARY("RuntimeObjectSystem.lib");


class Script : public TInterface<IID_IUPDATEABLE, IUpdateable>
{
public:
	virtual void Update(float deltaTime)
	{
	}
};