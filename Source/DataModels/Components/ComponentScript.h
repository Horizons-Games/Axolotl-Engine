#pragma once
#include "Auxiliar/Generics/Updatable.h"
#include "Component.h"

class Json;
class IScript;

class ComponentRigidBody;

class ComponentScript : public Component, public Updatable
{
public:
	ComponentScript(bool active, GameObject* owner);

	~ComponentScript() override;

	void Init();
	void Start();
	void PreUpdate() override;
	void Update() override;
	void PostUpdate() override;

	void OnCollisionEnter(ComponentRigidBody* other);
	void OnCollisionExit(ComponentRigidBody* other);

	void CleanUp();

	std::string GetConstructName() const;

	void SetConstuctor(const std::string& constructor);
	void SetScript(IScript* script);
	IScript* GetScript() const;

private:
	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	void SignalEnable() override;

private:
	// This will be managed by the runtime library
	IScript* script;
	std::string constructName;

	bool initialized = false;
	bool started = false;
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
