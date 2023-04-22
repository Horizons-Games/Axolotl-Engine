#pragma once
#include "Component.h"
#include "IScript.h"

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

	std::string GetConstructName() const;

	void SetConstuctor(const std::string& constructor);
	void SetScript(IScript* script);
	IScript* GetScript() const;

private:
	//This will be managed by the runtime library
	IScript* script;
	std::string constructName;
};


inline std::string ComponentScript::GetConstructName() const
{
	return constructName;
}

inline IScript* ComponentScript::GetScript() const
{
	return script;
}

inline void ComponentScript::SetScript(IScript* script)
{
	this->script = script;
}

inline void ComponentScript::SetConstuctor(const std::string& constructor)
{
	constructName = constructor;
}
