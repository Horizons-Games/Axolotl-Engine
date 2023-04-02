#pragma once
#include "Component.h"

#define COMPONENT_SCRIPT "Script"

class ComponentScript : public Component
{
public:
	ComponentScript(bool active, GameObject* owner);
	~ComponentScript() override;
	void Init() override;
	void Update() override;
private:
	std::string path;
	void LoadLibrary();
	void FreeLibrary();
};