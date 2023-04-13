#pragma once
#include "Component.h"

class Json;
class IScript;
class ResourceScript;

class ComponentScript : public Component
{
public:

	ComponentScript();

	ComponentScript(bool active, GameObject* owner);

	~ComponentScript() override;

	void Draw() override {};
	void Update() override {};

	void SetPath(const char* path);

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;
private:
	IScript* script;
	ResourceScript* resourceScript;
};

inline void ComponentScript::SetPath(const char* path){
	
}
