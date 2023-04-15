#pragma once
#include "Component.h"
#include "IScript.h"

class Json;
class IScript;


class ComponentScript : public Component
{
public:

	ComponentScript();

	ComponentScript(bool active, GameObject* owner);

	~ComponentScript() override;

	void Draw() override {};
	void Update() override {};


	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	const char* GetConstructName();

	void SetConstuctor(const char* constructor);
	void SetScript(IScript* script);
	IScript* GetScript();

	bool isInialized();
	bool isStarted();
	void Init() override;
private:
	bool initialized;
	bool started;
	IScript* script;
	const char* constructName;
};


inline const char* ComponentScript::GetConstructName() {
	return constructName;
}

inline IScript* ComponentScript::GetScript(){
	return script;
}

inline void ComponentScript::SetScript(IScript* script) {
	this->script = script;
}

inline bool ComponentScript::isInialized() {
	return initialized;
}

inline bool ComponentScript::isStarted() {
	return started;
}

inline void ComponentScript::Init()
{
	initialized = true;
	script->Init();
}

inline void ComponentScript::SetConstuctor(const char* constructor) {
	this->constructName = constructor;
}
