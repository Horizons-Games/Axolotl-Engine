#pragma once
#include "Component.h"
#include "IScript.h"

#include <memory>

class Json;
class IScript;


class ComponentScript : public Component
{
public:
	ComponentScript(bool active, GameObject* owner);

	~ComponentScript() override;

	virtual void Init() override;
	virtual void Start();
	virtual void PreUpdate();
	virtual void Update() override;
	virtual void PostUpdate();
	virtual void CleanUp();
	
	void Draw() override {};

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	std::string GetConstructName();

	void SetConstuctor(const std::string& constructor);
	void SetScript(IScript* script);
	IScript* GetScript();

private:
	//This will be managed by the runtime library
	IScript* script;
	std::string constructName;
};


inline std::string ComponentScript::GetConstructName()
{
	return constructName;
}

inline IScript* ComponentScript::GetScript()
{
	return script;
}

inline void ComponentScript::SetScript(IScript* script)
{
	this->script = script;
}

inline void ComponentScript::SetConstuctor(const std::string& constructor)
{
	this->constructName = constructor;
}
