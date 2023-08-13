#include "DefaultScript.h"
#include "DataModels/Components/ComponentTransform.h"
#include "DataModels/Components/ComponentAnimation.h"
#include "DataModels/Components/ComponentMeshCollider.h"
#include "DataModels/Components/ComponentScript.h"

#include "AxoLog.h"

REGISTERCLASS(DefaultScript);

DefaultScript::DefaultScript() : Script(), value(10), vecStr{ "Esto", "es un", "vector string", "de prueba" },
vecFloat{ 1.2f,2.3f,3.3f }, fl3(2.1f, 1.2f, 1.4f), vecFloat3{ fl3, fl3 }, vecGO{ (nullptr),(nullptr) }, vecBool{ (true),(false) },
sentence("Horizons"), character(nullptr), check(true)
{
	REGISTER_FIELD(value, float);
	REGISTER_FIELD(sentence, std::string);
	REGISTER_FIELD_WITH_ACCESSORS(Float3, float3);
	REGISTER_FIELD_WITH_ACCESSORS(VectorFloat, std::vector<float>);
	REGISTER_FIELD_WITH_ACCESSORS(VectorFloat3, std::vector<float3>);
	REGISTER_FIELD_WITH_ACCESSORS(VectorStr, std::vector<std::string>);
	REGISTER_FIELD_WITH_ACCESSORS(VectorGO, std::vector<GameObject*>);
	REGISTER_FIELD_WITH_ACCESSORS(VectorBool, std::vector<bool>);
	REGISTER_FIELD_WITH_ACCESSORS(Character, GameObject*);
	REGISTER_FIELD(check, bool);
	REGISTER_FIELD(transform, ComponentTransform*);
	REGISTER_FIELD_WITH_ACCESSORS(Script, DefaultScript*);
	REGISTER_FIELD(colliders, std::vector<ComponentMeshCollider*>);
	REGISTER_FIELD_WITH_ACCESSORS(Animations, std::vector<ComponentAnimation*>);
}

void DefaultScript::Update(float deltaTime)
{
	LOG_DEBUG("{}", value);
	LOG_DEBUG("{} {} {} ", fl3[2], fl3[1], fl3[0]);
	LOG_DEBUG("{}", sentence.c_str());

	if (character != nullptr)
	{
		LOG_DEBUG("{}", character);
	}

	LOG_DEBUG("{}", check);
}

//Float3
const float3& DefaultScript::GetFloat3() const
{
	return fl3;
}

void DefaultScript::SetFloat3(const float3& fl3)
{
	this->fl3 = fl3;
}

//Vectors
const std::vector<float>& DefaultScript::GetVectorFloat() const
{
	return vecFloat;
}

void DefaultScript::SetVectorFloat(const std::vector<float>& vecFloat)
{
	this->vecFloat = vecFloat;
}

const std::vector<float3>& DefaultScript::GetVectorFloat3() const
{
	return vecFloat3;
}

void DefaultScript::SetVectorFloat3(const std::vector<float3>& vecFloat3)
{
	this->vecFloat3 = vecFloat3;
}

const std::vector<std::string>& DefaultScript::GetVectorStr() const
{
	return vecStr;
}

void DefaultScript::SetVectorStr(const std::vector<std::string>& vecStr)
{
	this->vecStr = vecStr;
}

const std::vector<GameObject*>& DefaultScript::GetVectorGO() const
{
	return vecGO;
}

void DefaultScript::SetVectorGO(const std::vector<GameObject*>& vecGO)
{
	this->vecGO = vecGO;
}

const std::vector<bool>& DefaultScript::GetVectorBool() const
{
	return vecBool;
}

void DefaultScript::SetVectorBool(const std::vector<bool>& vecBool)
{
	this->vecBool = vecBool;
}

//GameObject
GameObject* DefaultScript::GetCharacter() const
{
	return character;
}

void DefaultScript::SetCharacter(GameObject* character)
{
	this->character = character;
}

DefaultScript* DefaultScript::GetScript() const
{
	return script;
}

void DefaultScript::SetScript(DefaultScript* script)
{
	this->script = script;
}

const std::vector<ComponentAnimation*>& DefaultScript::GetAnimations() const
{
	return animations;
}

void DefaultScript::SetAnimations(const std::vector<ComponentAnimation*>& animations)
{
	this->animations = animations;
}
