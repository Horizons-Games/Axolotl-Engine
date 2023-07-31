#pragma once
#include "IScript.h"
#include "ISimpleSerializer.h"
#include "InterfacesIds.h"
#include "ObjectInterfacePerModule.h"

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

	const std::vector<TypeFieldPair>& GetFields() const override;

	void Serialize(ISimpleSerializer* pSerializer) override;

protected:
	void AddMember(TypeFieldPair&& member);

protected:
	GameObject* owner;
	Application* App;

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

inline const std::vector<TypeFieldPair>& Script::GetFields() const
{
	return members;
}
