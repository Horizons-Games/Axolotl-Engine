#pragma once
#include "Scripting\Script.h"
#include "RuntimeInclude.h"
#include "Math/float3.h"
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

	float GetValue() const;
	void SetValue(float value);

	float3 GetVector3() const;
	void SetVector3(float3 vec3);

	std::string GetSentence() const;
	void SetSentence(const std::string& sentence);

	GameObject* GetCharacter() const;
	void SetCharacter(GameObject* character);

	bool GetCheck() const;
	void SetCheck(bool check);

private:
	float value;
	float3 vec3;
	std::string sentence;
	GameObject* character;
	bool check;
};