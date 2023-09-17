#pragma once
#include "IScript.h"

#include "RuntimeObjectSystem/ISimpleSerializer.h"
#include "RuntimeObjectSystem/ObjectInterfacePerModule.h"

#include "Scripting/InterfacesIds.h"
#include "Scripting/RegisterFieldMacros.h"

#include "RuntimeLinkLibrary.h"
RUNTIME_COMPILER_LINKLIBRARY("ExternalDependencies.lib");
RUNTIME_COMPILER_LINKLIBRARY("Engine.lib");
RUNTIME_COMPILER_LINKLIBRARY("OptickCore.lib");
RUNTIME_COMPILER_LINKLIBRARY("DirectXTex.lib");

class Script : public TInterface<IID_IUPDATEABLE, IScript>
{
public:
	Script();
	virtual ~Script() override = default;

	virtual void Init() override{};
	virtual void Start() override{};
	virtual void PreUpdate(float deltaTime) override{};
	virtual void Update(float deltaTime) override{};
	virtual void PostUpdate(float deltaTime) override{};
	virtual void OnCollisionEnter(ComponentRigidBody* other) override{};
	virtual void OnCollisionExit(ComponentRigidBody* other) override{};
	virtual void CleanUp() override{};

	GameObject* GetOwner() const override;
	void SetOwner(GameObject* owner) override;

	ComponentScript* GetContainer() const override;
	void SetContainer(ComponentScript* container) override;
	void Enable() override;
	void Disable() override;

	const std::vector<TypeFieldPair>& GetFields() const override;

	void Serialize(ISimpleSerializer* pSerializer) override;

protected:
	void AddMember(TypeFieldPair&& member);

	void Assert(bool&& condition, std::string&& errorMessage);

protected:
	GameObject* owner;
	Application* App;
	ComponentScript* container;

private:
	std::vector<TypeFieldPair> members;
};

inline GameObject* Script::GetOwner() const
{
	return owner;
}

inline void Script::SetOwner(GameObject* owner)
{
	this->owner = owner;
}

inline ComponentScript* Script::GetContainer() const
{
	return container;
}

inline void Script::SetContainer(ComponentScript* container)
{
	this->container = container;
}

inline const std::vector<TypeFieldPair>& Script::GetFields() const
{
	return members;
}
