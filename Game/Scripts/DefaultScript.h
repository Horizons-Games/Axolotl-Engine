#pragma once

#include "Scripting\Script.h"
#include "RuntimeInclude.h"

RUNTIME_MODIFIABLE_INCLUDE;

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

	const float3& GetVector3() const;
	void SetVector3(const float3& vec3);

	GameObject* GetCharacter() const;
	void SetCharacter(GameObject* character);

private:
	float value;
	float3 vec3;
	std::string sentence;
	GameObject* character;
	bool check;
};