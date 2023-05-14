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
	void CleanUp() override {};

	GameObject* GetCharacter() const;
	void SetCharacter(GameObject* character);

private:
	float value;
	std::string sentence;
	GameObject* character;
	bool check;
};