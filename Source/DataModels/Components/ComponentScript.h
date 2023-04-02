#pragma once
#include "Component.h"
#include <memory>
#include <windows.h>


#define COMPONENT_SCRIPT "Script"

class Application;

class ComponentScript : public Component
{
public:
	ComponentScript(bool active, GameObject* owner, const char* path);
	~ComponentScript() override;
	void Init() override;
	void Update() override;
private:
	typedef void* (*initScript)(GameObject* owner);
	typedef void* (*updateScript)(GameObject* owner);
	const char* path;
	void LoadLibraryScript();
	void FreeLibraryScript();
	initScript scriptInit;
	updateScript scriptUpdate;
	HINSTANCE library;
};