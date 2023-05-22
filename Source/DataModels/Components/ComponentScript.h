#pragma once
#include "Auxiliar/Generics/Updatable.h"
#include "Component.h"

class Json;
class IScript;

class ComponentScript : public Component, public Updatable
{
public:
	ComponentScript(bool active, GameObject* owner);

	~ComponentScript() override;

	virtual void Init();
	virtual void Start();
	virtual void PreUpdate() override;
	virtual void Update() override;
	virtual void PostUpdate() override;
	virtual void CleanUp();

	void SaveOptions(Json& meta) override;
	void LoadOptions(Json& meta) override;

	std::string GetConstructName() const;

	void SetConstuctor(const std::string& constructor);
	void SetScript(IScript* script);
	IScript* GetScript() const;

private:
	// This will be managed by the runtime library
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
