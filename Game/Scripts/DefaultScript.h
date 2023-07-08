#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

class ComponentTransform;
class HelloWorldScript;

// This script is just to show how to instantiate/use the different types of variables

class DefaultScript : public Script
{
public:
	DefaultScript();
	~DefaultScript() override = default;

	void Init() override {};
	void Start() override {};
	void PreUpdate(float deltaTime) override {};
	void Update(float deltaTime) override;
	void PostUpdate(float deltaTime) override {};
	virtual void OnCollisionEnter(ComponentRigidBody* other) override {};
	virtual void OnCollisionExit(ComponentRigidBody* other) override {};
	void CleanUp() override {};

	const float3& GetFloat3() const;
	void SetFloat3(const float3& fl3);

	const std::vector<float>& GetVectorFloat() const;
	void SetVectorFloat(const std::vector<float>& vecFloat);
	const std::vector<float3>& GetVectorFloat3() const;
	void SetVectorFloat3(const std::vector<float3>& vecFloat3);
	const std::vector<std::string>& GetVectorStr() const;
	void SetVectorStr(const std::vector<std::string>& vecStr);
	const std::vector<GameObject*>& GetVectorGO() const;
	void SetVectorGO(const std::vector<GameObject*>& vecGO);
	const std::vector<bool>& GetVectorBool() const;
	void SetVectorBool(const std::vector<bool>& vecBool);

	GameObject* GetCharacter() const;
	void SetCharacter(GameObject* character);

	DefaultScript* GetScript() const;
	void SetScript(DefaultScript* script);

private:
	float value;
	float3 fl3;
	std::vector<float> vecFloat;
	std::vector<float3> vecFloat3;
	std::vector<std::string> vecStr;
	std::vector<GameObject*> vecGO;
	std::vector<bool> vecBool;
	std::string sentence;
	GameObject* character;
	ComponentTransform* transform;
	DefaultScript* script;
	bool check;
};