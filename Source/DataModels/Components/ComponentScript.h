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
	ComponentScript(const ComponentScript& other);

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

	void SetOwner(GameObject* owner) override;

private:
	bool ScriptCanBeCalled() const;

	void InternalSave(Json& meta) override;
	void InternalLoad(const Json& meta) override;

	void SignalEnable() override;

public:
	// When loading a Script, it's possible that it expects to find one in a given game object (when it has that Script as a Field)
	// thats why we need to instantiate them all before loading
	void InstantiateScript(const Json& jsonComponent);

private:
	// This will be managed by the runtime library
	IScript* script;
	std::string constructName;

	bool initialized = false;
	bool started = false;

	// whether this component raised an error during its execution
	bool failed = false;
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
